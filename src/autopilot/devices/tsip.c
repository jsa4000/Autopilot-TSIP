



void tsip_listen (void)
{
    int dle_active = 0;
    Tsip.index = 0;
    byte b, buf[1000];
    long byte_count;
    int i;

    // Loop through buffer a byte at a time, handling DLE state.
    //   1. DLE ETX  -- end of packet
    //   2. DLE <c>  -- escape c
    //   3. DLE      -- one of above
    //   4. <c>      -- normal data

    for (i = 0; i < byte_count; i += 1) {
        b = buf[i];
        if (dle_active) {
            if (b == ETX) {

                // 1. End of packet detected (DLE ETX).

                tsip_triage();
                Tsip.index = 0;

            } else {

                // 2. Store escaped character.

                Tsip.data[Tsip.index++] = b;
            }
            dle_active = 0;

        } else {
            if (b == DLE) {

                // 3. Mark DLE state and get next character.

                dle_active = 1;

            } else {

                // 4. Store normal data byte.

                Tsip.data[Tsip.index++] = b;
            }
        }

        // Buffer overflow will occur for bogus non-TSIP serial data.

        if (Tsip.index >= sizeof Tsip.data) {
            fprintf(stderr, "** lots of non-TSIP data detected\n");
            Tsip.index = 0;
        }
    }
    
}

//
// Send TSIP packet of the given length.
//
void tsip_send (int length, ...)
{
    byte buf[100], b;
    int size = 0;
    va_list vap;

    buf[size++] = DLE;
    va_start(vap, length);
    while (length--) {
        b = va_arg(vap, byte);
        if (b == DLE) {
            buf[size++] = DLE;
        }
        buf[size++] = b;
    }
    va_end(vap);
    buf[size++] = DLE;
    buf[size++] = ETX;
    CommWrite(buf, size);

    if (opt.debug) {
        int i;
        fprintf(stderr, "tsip_send = [ 0x");
        for (i = 0; i < size; i++) {
            fprintf(stderr, "%.2X ", buf[i]);
        }
        fprintf(stderr, "]\n");
    }
}

//
// Send TSIP packets.
// - commands are sent so that responses can be displayed
// - called once per received packet
// - check if any commands should be sent
//
void tsip_talk (void)
{
    char c;

    if (opt.use_stdin) {
        return;
    }
    fflush(stdout);

    // Automatically send commands a few seconds after start.

    switch (stat.packets) {
    case 1 :
        tsip_send(1, 0x1F);         // request software version
        break;

    case 2 :
        tsip_send(2, 0x8E, 0xA9);   // self-survey parameters
        tsip_send(2, 0x8E, 0x41);   // manufacturing operating parameters
        tsip_send(2, 0x8E, 0x42);   // production parameters
        break;

    case 3 :
        tsip_send(2, 0x8E, 0xA0);   // DAC voltage
        tsip_send(2, 0x8E, 0xA1);   // 10 MHz sense
        tsip_send(2, 0x8E, 0xA2);   // UTC/GPS timing
     // tsip_send(2, 0x8E, 0xA8, 0); // loop dynamics
     // tsip_send(2, 0x8E, 0xA8, 1); // oscillator parameters
     // tsip_send(2, 0x8E, 0xA8, 2); // recovery parameters
        break;
    }

    // Check for user input (small set of one-letter commands).

    while (_kbhit()) {
        c = _getch();
        Lprintf("** user command: '%c' (0x%.2X)\n", c, c);
        switch (c) {
        case 'h' :
        case '?' :
            fprintf(stderr, "One letter commands:\n");
            fprintf(stderr, "  ? - help (this message)\n");
            fprintf(stderr, "  J - (0) jam sync\n");
            fprintf(stderr, "  R - (1) recovery\n");
            fprintf(stderr, "  H - (2) into holdover\n");
            fprintf(stderr, "  O - (3) out of holdover\n");
            fprintf(stderr, "  D - (4) disable discipline\n");
            fprintf(stderr, "  E - (5) enable discipline\n");
            fprintf(stderr, "  K - cold reset\n");
            fprintf(stderr, "  F - factory reset\n");
            fprintf(stderr, "  U - enable UTC data\n");
            fprintf(stderr, "  v - toggle verbose\n");
            fprintf(stderr, "  q - quit program\n");
            fprintf(stderr, "\n");
            break;

        case 'J' : tsip_send(3, 0x8E, 0xA3, 0); break;
        case 'R' : tsip_send(3, 0x8E, 0xA3, 1); break;
        case 'H' : tsip_send(3, 0x8E, 0xA3, 2); break;
        case 'O' : tsip_send(3, 0x8E, 0xA3, 3); break;
        case 'D' : tsip_send(3, 0x8E, 0xA3, 4); break;
        case 'E' : tsip_send(3, 0x8E, 0xA3, 5); break;
        case 'K' : tsip_send(2, 0x1E, 'K'); break;
        case 'F' : tsip_send(2, 0x1E, 'F'); break;
        case 'U' : tsip_send(4, 0x38, 1, 5, 0); break; // enable UTC data

        case 'v' :
            opt.verbose ^= 1;
            LogQuiet = opt.verbose;
            break;

        case 'q' :
            quit();
            break;

        default :
            fprintf(stderr, "** ignoring user key '%c' = 0x%.2X\n", c, c);
        }
    }

    // Once a minute requests.

    {
        static time_t next;
        time_t now = time(NULL);

        if (now > next) {
            tsip_send(4, 0x38, 1, 5, 0);
            next = now + 60;
        }
    }
}



//
// The following code allows easy decoding and display of TSIP packets.
// Simple per-packet functions extract byte-reordered fields in sequence
// from a global packet buffer into local variables which can then be
// formatted for output.
//
int endian = 1;
#define LITTLE (*(char *)&endian == 1)
void *tsip_bytes (int n)
{
    static byte buf[8];
    int i;

    // Extract multibyte field from packet, honoring byte order.
    for (i = 0; i < n; i += 1) {
        buf[LITTLE ? (n - 1 - i) : (i)] = Tsip.data[Tsip.index++];
    }
    return buf; // WARNING: static buffer
}

void tsip_stuff (byte *buf, int n)
{
    int i;

    for (i = 0; i < n; i += 1) {
        Tsip.data[Tsip.index++] = buf[LITTLE ? (n - 1 - i) : (i)];
    }
}

//
// Identify received TSIP packet type and call handler functions above.
//
void tsip_triage (void)
{
    int size;
    byte pid = Tsip.data[0];
    byte sub = Tsip.data[1];

    // Note: size does not include ETX or any DLE.
    size = Tsip.index;
    Tsip.index = 0;

    stat.packets += 1;

    // Identify packets by packet id.
    switch (pid) {
    case 0x42 : tsip_42(); break;
    case 0x43 : tsip_43(); break;
    case 0x45 : tsip_45(); break;
    case 0x47 : tsip_47(); break;
    case 0x58 : tsip_58(); break;
    case 0x5A : tsip_5A(); break;
    case 0x5B : tsip_5B(); break;
    case 0x6D : tsip_6D(); break;

    case 0x8F :
        // Identify 8F packets by subcode.
        switch (sub) {
        case 0x41 : tsip_8F_41(); break;
        case 0x42 : tsip_8F_42(); break;
        case 0xA0 : tsip_8F_A0(); break;
        case 0xA1 : tsip_8F_A1(); break;
        case 0xA2 : tsip_8F_A2(); break;
        case 0xA3 : tsip_8F_A3(); break;
        case 0xA7 : tsip_8F_A7(size); break;
        case 0xA9 : tsip_8F_A9(); break;
        case 0xAB : tsip_8F_AB(); break;
        case 0xAC : tsip_8F_AC(); break;
        default :
            Lprintf("** 0x%.2X-%.2X TSIP packet not decoded\n", pid, sub);
            stat.stray += 1;
            Tsip.index = size;
            break;
        }
        break;

    default :
        Lprintf("** 0x%.2X TSIP packet not decoded\n", pid);
        stat.stray += 1;
        Tsip.index = size;
        break;
    }

    // Reset buffer for next receive packet.

    if (Tsip.index != size) {
        fprintf(stderr, "** packet size mismatch? (index %d size %d) %.2x %.2x %.2x %.2x %.2x\n", Tsip.index, size,
            Tsip.data[0], Tsip.data[1], Tsip.data[2], Tsip.data[3], Tsip.data[4]);
        if (opt.stop) {
            fprintf(stderr, "** stop\n");
            exit(0);
        }
    }

    // Check if we should send a packet.

    tsip_talk();
}

