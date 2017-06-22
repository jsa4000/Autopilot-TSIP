//
// tbtalk.exe - Command line talk/listen tool for Trimble Thunderbolt GPSDO.
//
// TSIP protocol notes:
// - RS-232 serial interface at 9600 baud 8-N-1
// - binary interface (not NMEA)
// - two byte header is DLE + pid (packet id)
// - two byte trailer is DLE + ETX (no length, checksum, or <cr><lf>)
// - any embedded DLE data bytes are escaped with DLE
// - multibyte fields are unaligned and big-endian
//
// 28-Jul-2009 Tom Van Baak (tvb) www.LeapSecond.com/tools
//

#include <conio.h>
#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Use this serial port and data logging code.

#include "../com_lib.c"
#include "../log_lib.c"

// TSIP byte definitions.

typedef unsigned char byte;
#define ETX 0x03
#define DLE 0x10

// Define function prototypes, global data, macros.

void tsip_listen (void);
void tsip_talk (void);
void tsip_triage (void);
#define MAX_CHAN 16
#define NSV 32

#define Y2K_ADJUST(y) ( (((y) < 80) ? 2000 : 1900) + (long)(y) )
#define PI 3.1415926535898 // ICD-GPS-200 version of pi
#define DEG(rad) (180.0 * (rad) / PI)
#define DO_CHECKS 1

// Define global data for handling _current_ packet.

struct {
    byte data[1000];    // TSIP data buffer
    long index;         // current index into buffer
} Tsip;

// Program statistics.

struct {
    long bytes;
    long reads;
    long packets;
    long stray;
    long seconds;
} stat;

// Program options.

struct {
    int comport;
    char *logpath;
    char *rawpath;
    FILE *raw_file;
    int logtype;
    int verbose;
    int use_stdin;
    long max_sec;
    long time_out;
    int debug;
    int hex;
    int flow;
    int stop;
} opt;

char Usage[] =
    "Command line talk/listen tool for Trimble Thunderbolt GPSDO\n"
    "\n"
    "Usage: tbtalk [options] [com_port] [log_path]\n"
    "\n"
    "Options:\n"
    "    /max:#  - limit maximum seconds to collect data\n"
    "    /raw:f  - save raw TSIP binary to output file\n"
    "    /q      - quiet (no display, log only)\n"
    "    /v      - verbose\n"
    "    /debug  - debug\n"
    "    /log    - show available log time-stamp options\n"
    "    /help   - help (or /? or /h)\n"
    "\n"
    "Examples:\n"
    "    tbtalk 4\n"
    "    tbtalk /max:60 11 tbolt.txt\n"
    "    tbtalk - <tbolt.bin\n"
    ;

// Get value of keyword (the substring following the colon).
#define VALUE(keyword) (strchr((keyword), ':') + 1)

// Match keyword-with-value (e.g., /port:11) or match keyword-only (e.g., /v).
int MATCH (char *s, char *key)
{
    return strchr(key,':') ? !strncmp(key,s,VALUE(key) - key) : !strcmp(key,s);
}

int get_options (int argc, char *argv[])
{
    char *arg;
    int n = 0;

    while (argc > 1 && argv[1][0] == '/') {
        arg = argv[1];
        if (MATCH(arg, "/?") ||
            MATCH(arg, "/h") ||
            MATCH(arg, "/help")) {
            fprintf(stderr, Usage);
            exit(0);
        } else

        if (MATCH(arg, "/max:")) {
            opt.max_sec = atol(VALUE(arg));
        } else

        if (MATCH(arg, "/raw:")) {
            opt.rawpath = VALUE(arg);
        } else

        if (MATCH(arg, "/to:")) {
            opt.time_out = atol(VALUE(arg));
        } else

        if (LogType(arg, &opt.logtype)) {
            ;
        } else

        if (MATCH(arg, "/q")) {
            LogQuiet = 1;
        } else

        if (MATCH(arg, "/v")) {
            opt.verbose = 1;
        } else

        if (MATCH(arg, "/hex")) {
            opt.hex = 1;
        } else

        if (MATCH(arg, "/stop")) {
            opt.stop = 1;
        } else

        if (MATCH(arg, "/flow")) {
            opt.flow ^= 1;
        } else

        if (MATCH(arg, "/debug")) {
            opt.debug = 1;
        } else

        {
            fprintf(stderr, "Unknown option: %s\n", arg);
            exit(1);
        }
        argc -= 1;
        argv += 1;
        n += 1;
    }

    return n;
}

// Define printf function that also time-stamps and logs to a file.

void Lprintf (char *format, ...)
{
    char buf[1000];
    va_list vap;

    strcpy(buf, "");
    va_start(vap, format);
    vsprintf(buf, format, vap);
    va_end(vap);

    LogTimeStamp(opt.logtype);
    LogWrite(buf, strlen(buf));
}

// Terminate program.

void quit (void)
{
    fprintf(stderr, "** ", stat.bytes);
    fprintf(stderr, "%ld TSIP bytes, ", stat.bytes);
    fprintf(stderr, "%ld data reads, ", stat.reads);
    fprintf(stderr, "%ld packets, ", stat.packets);
    fprintf(stderr, "%ld stray, ", stat.stray);
    fprintf(stderr, "%ld seconds", stat.seconds);
    fprintf(stderr, ".\n");
    LogClose();
    exit(0);
}

// Main program.

void main (int argc, char *argv[])
{
    int n;

    n = get_options(argc, argv);
    argc -= n;
    argv += n;

    if (argc < 2) {
        fprintf(stderr, Usage);
        exit(1);
    }

    // First argument is COM port.

    opt.comport = 1;
    if (argc > 1) {
        if (strcmp(argv[1], "-") == 0) {
            opt.use_stdin = 1;
        } else {
            opt.comport = atoi(argv[1]);
        }
    }

    // Second argument is log file (default is auto-generated name).

    opt.logpath = NULL;
    if (argc > 2) {
        opt.logpath = argv[2];
    }

    // Open serial port (or stdin).

    if (opt.use_stdin) {
        // Avoid ^Z from terminating stream input.
        _setmode(_fileno(stdin), _O_RDONLY | _O_BINARY);

    } else {
        CommOpen(opt.comport, 9600, 0, opt.flow);
        if (opt.time_out == 0) opt.time_out = 50;
        fprintf(stderr, "time_out = %ld flow = %d\n", opt.time_out, opt.flow);
        CommTimeout(opt.time_out, 0, 0);
    }

    // Open log file.

    LogOpen(opt.logpath);

    // Open optional raw TSIP binary output file.

    if (opt.rawpath != NULL) {
        opt.raw_file = fopen(opt.rawpath, "wb"); // append
        if (NULL == opt.raw_file) {
            fprintf(stderr, "Open failed: %s\n", opt.rawpath);
            exit(1);
        }
    }

    // Read data and process TSIP packets.

    tsip_listen();
}

void tsip_listen (void)
{
    int dle_active = 0;

    Tsip.index = 0;
    for (;;) {
        byte b, buf[1000];
        long byte_count;
        int i;

        // Get next batch of bytes from serial port.

        if (opt.use_stdin) {
            int c;
            if (EOF == (c = getchar())) {
                fprintf(stderr, "** EOF at 0x%.8lX\n", stat.reads);
                quit();
            }
            buf[0] = (byte) c;
            byte_count = 1;

        } else {
            byte_count = CommRead(buf, sizeof buf);
            if (opt.raw_file) {
                fwrite(buf, byte_count, 1, opt.raw_file);
                fflush(opt.raw_file);
            }
        }
        stat.bytes += byte_count;
        stat.reads += 1;

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

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef signed char sint8;
typedef signed short sint16;
typedef signed long sint32;

uint8  tsip_sint8 () { return *(sint8  *)tsip_bytes(1); }
uint8  tsip_uint8 () { return *(uint8  *)tsip_bytes(1); }
sint16 tsip_sint16() { return *(sint16 *)tsip_bytes(2); }
uint16 tsip_uint16() { return *(uint16 *)tsip_bytes(2); }
uint32 tsip_uint32() { return *(uint32 *)tsip_bytes(4); }
float  tsip_float () { return *(float  *)tsip_bytes(4); }
double tsip_double() { return *(double *)tsip_bytes(8); }

void put_uint8  (uint8  v) { tsip_stuff((byte *)&v, sizeof v); }
void put_uint16 (uint16 v) { tsip_stuff((byte *)&v, sizeof v); }
void put_uint32 (uint32 v) { tsip_stuff((byte *)&v, sizeof v); }
void put_float  (float  v) { tsip_stuff((byte *)&v, sizeof v); }
void put_double (double v) { tsip_stuff((byte *)&v, sizeof v); }

// TSIP documentaion from: ThunderBoltBook2003.pdf

//
// A.9.19 Report Packet 0x42 Single-precision Position Fix.
//
void tsip_42 (void)
{
    uint8  pid         = tsip_uint8();
    float  x           = tsip_float();
    float  y           = tsip_float();
    float  z           = tsip_float();
    float  time_of_fix = tsip_float();

    Lprintf("0x%.2X: %.2f x, %.2f y, %.2f z, %g TOF\n",
            pid, x, y, z, time_of_fix);
}

//
// A.9.20 Report Packet 0x43 Velocity Fix, XYZ ECEF.
//
void tsip_43 (void)
{
    uint8  pid         = tsip_uint8();
    float  x_velocity  = tsip_float();
    float  y_velocity  = tsip_float();
    float  z_velocity  = tsip_float();
    float  bias_rate   = tsip_float();
    float  time_of_fix = tsip_float();

    Lprintf("0x%.2X: %g Xv, %g Yv, %g Zv, %g bias, %g TOF\n",
            pid, x_velocity, y_velocity, z_velocity, bias_rate, time_of_fix);
}

//
// A.9.21 Report Packet Software Version Information.
//  - this packet is sent at power-on and resets
//  - this packet is sent in response to packet 0x1F
//
void tsip_45 (void)
{
    uint8  pid                                    = tsip_uint8();
    uint8  application_layer_major_version_number = tsip_uint8();
    uint8  application_layer_minor_version_number = tsip_uint8();
    uint8  app_month                              = tsip_uint8();
    uint8  app_day                                = tsip_uint8();
    uint8  app_year_minus_1900                    = tsip_uint8();
    uint8  gps_core_layer_major_version_number    = tsip_uint8();
    uint8  gps_core_layer_minor_version_number    = tsip_uint8();
    uint8  gps_month                              = tsip_uint8();
    uint8  gps_day                                = tsip_uint8();
    uint8  gps_year_minus_1900                    = tsip_uint8();

    Lprintf("0x%.2X: APP VER: %2u.%-2u DATE: %.4u-%.2u-%.2u\n",
            pid,
            application_layer_major_version_number,
            application_layer_minor_version_number,
            Y2K_ADJUST(app_year_minus_1900),
            app_month,
            app_day);

    Lprintf("0x%.2X: GPS VER: %2u.%-2u DATE: %.4u-%.2u-%.2u\n",
            pid,
            gps_core_layer_major_version_number,
            gps_core_layer_minor_version_number,
            Y2K_ADJUST(gps_year_minus_1900),
            gps_month,
            gps_day);
}

//
// A.9.22 Report Packet 0x47 Signal Levels for All Satellites Tracked
//  - the receiver sends this packet only in response to packet 0x27
//
void tsip_47 (void)
{
    uint8  pid   = tsip_uint8();
    uint8  count = tsip_uint8();
    uint8  satellite_number[MAX_CHAN];
    float  signal_level[MAX_CHAN];
    char buf[1000];

    strcpy(buf, "");
    if (count > 0 && count <= MAX_CHAN) {
        int i;
        char *p = buf;
        for (i = 0; i < count; i += 1) {
            satellite_number[i] = tsip_uint8();
            signal_level[i] = tsip_float();
            p += sprintf(p, "%d:%g ", satellite_number[i], signal_level[i]);
        }
    }
    Lprintf("0x%.2X: %d nsv, [ %s]\n", pid, count, buf);
}

//
// A.9.28 Report Packet 0x58 GPS System Data from Receiver
//  - the receiver sends this packet in response to packet 0x38
//
void tsip_58 (void)
{
    uint8  pid          = tsip_uint8();
    uint8  operation    = tsip_uint8();
    uint8  type_of_data = tsip_uint8();
    uint8  prn          = tsip_uint8();
    uint8  length       = tsip_uint8();

    Lprintf("0x%.2X: %d op, %d type, %d prn, %d length\n",
            pid,
            operation,
            type_of_data,
            prn,
            length);

    if (type_of_data == 2) {
        uint8  t_oa_raw     = tsip_uint8();
        uint8  sv_health    = tsip_uint8();
        float  e            = tsip_float();
        float  t_oa         = tsip_float();
        float  i_o          = tsip_float();
        float  omega_dot    = tsip_float();
        float  a            = tsip_float();
        float  omega_0      = tsip_float();
        float  what         = tsip_float();
        float  m_0          = tsip_float();
        float  a_f0         = tsip_float();

        float  a_f1         = tsip_float();
        float  a_what       = tsip_float();
        float  n_what       = tsip_float();
        float  omega_n      = tsip_float();
        float  odot_n       = tsip_float();
        float  t_zc         = tsip_float();

        uint16 WN           = tsip_uint16();
        uint16 WNa          = tsip_uint16();

        Lprintf("0x%.2X:%d FIX type_of_data == 2\n", pid, type_of_data);
    }

    if (type_of_data == 3) {
        uint8  wn_health    = tsip_uint8();
        uint8 sv_health[NSV];
        int i;
        for (i = 0; i < NSV; i += 1) {
            sv_health[i] = tsip_uint8();
        }
        {
            uint8  t_oa_health   = tsip_uint8();
            uint8  current_t_oa  = tsip_uint8();
            uint16 wn           = tsip_uint16();
        }

        Lprintf("0x%.2X:%d FIX type_of_data == 3\n", pid, type_of_data);
    }

    if (type_of_data == 4) {
        uint32 unused1 = tsip_uint32();
        uint32 unused2 = tsip_uint32();
        float  i0      = tsip_float();
        float  i1      = tsip_float();
        float  i2      = tsip_float();
        float  i3      = tsip_float();
        float  j0      = tsip_float();
        float  j1      = tsip_float();
        float  j2      = tsip_float();
        float  j3      = tsip_float();
        Lprintf("0x%.2X:%d %g %g %g %g %g %g %g %g\n", pid, type_of_data,
                i0, i1, i2, i3, j0, j1, j2, j3);
    }

    if (type_of_data == 5) {
        uint8  unused1 = tsip_uint8();
        uint32 unused2 = tsip_uint32();
        uint32 unused3 = tsip_uint32();
        uint32 unused4 = tsip_uint32();
        double A0      = tsip_double();
        float  A1      = tsip_float();
        sint16 tLS     = tsip_sint16();
        float  tot     = tsip_float();
        uint16 WNt     = tsip_uint16();
        uint16 WNlsf   = tsip_uint16();
        uint16 DN      = tsip_uint16();
        sint16 tLSf    = tsip_sint16();
        long intA0 = (long) (A0 * pow(2, 30)); // per GPS ICD
        long intA1 = (long) (A1 * pow(2, 50)); // per GPS ICD

        Lprintf("0x%.2X:%da %.12le A0(%ld), %.12e A1(%ld), %d tLS\n",
                pid, type_of_data,
                A0, intA0, A1, intA1, tLS);
        Lprintf("0x%.2X:%db %lg tot, %u WNt, %u WNlsf, %u DN, %d tLSf\n",
                pid, type_of_data,
                tot, WNt, WNlsf, DN, tLSf);
    }

    if (type_of_data == 6) {
        uint8  svn              = tsip_uint8();
        float  t_ephem          = tsip_float();
        uint16 wn               = tsip_uint16();
        uint8  unused1          = tsip_uint8();
        uint8  unused2          = tsip_uint8();
        uint8  sv_accuracy_raw  = tsip_uint8();
        uint8  sv_health        = tsip_uint8();
        uint16 iodc             = tsip_uint16();

        float  t_gd             = tsip_float();
        float  t_oc             = tsip_float();
        float  a_f2             = tsip_float();
        float  a_f1             = tsip_float();
        float  a_f0             = tsip_float();
        float  sv_accuracy      = tsip_float();

        uint8  iode             = tsip_uint8();
        uint8  unused3          = tsip_uint8();
        float  c_rs             = tsip_float();
        float  _n               = tsip_float();
        double m_0              = tsip_double();

        float  c_uc             = tsip_float();
        double e                = tsip_double();
        float  c_us             = tsip_float();
        double a                = tsip_double();
        float  t_oe             = tsip_float();
        float  c_ic             = tsip_float();

        double omega_0          = tsip_double();
        float  c_is             = tsip_float();
        double i_o              = tsip_double();
        float  c_rc             = tsip_float();
        double what             = tsip_double();
        float  omega_dot        = tsip_float();

        float  idot             = tsip_float();
        double a_2              = tsip_double();
        double n                = tsip_double();
        double r1me2            = tsip_double();
        double omega_n          = tsip_double();
        double odot_n           = tsip_double();

        Lprintf("0x%.2X:%da %g t_ephem, %d wn, %x ?, %x ?, %d acc_raw, %d health, %d iodc\n",
                pid, type_of_data,
                t_ephem, wn, unused1, unused2, sv_accuracy_raw, sv_health, iodc);
        Lprintf("0x%.2X:%db %g %g %g %g %g %g\n",
                pid, type_of_data,
                t_gd, t_oc, a_f2, a_f1, a_f0, sv_accuracy);
        Lprintf("0x%.2X:%dc %d iode, %d ?, %g c_rs, %g _n, %lg m_0\n",
                pid, type_of_data,
                iode, unused3, c_rs, _n, m_0);
        Lprintf("0x%.2X:%dd %g c_uc, %lg e, %g c_us, %lg a, %g t_oe, %g c_ic\n",
                pid, type_of_data,
                c_uc, e, c_us, a, t_oe, c_ic);
        Lprintf("0x%.2X:%de %lg omega_0, %g c_is, %lg i_o, %g c_rc, %lg ?, %g omega_dot\n",
                pid, type_of_data,
                omega_0, c_is, i_o, c_rc, what, omega_dot);
        Lprintf("0x%.2X:%df %g idot, %lg a_2, %lg n, %lg r1me2, %lg omega_n, %lg odot_n\n",
                pid, type_of_data,
                idot, a_2, n, r1me2, omega_n, odot_n);
    }
}

//
// A.9.30 Report Packet 0x5A Raw Measurement Data
// - Packet 0x5A provides raw GPS measurement data.
// - If the packet 0x35 auxiliary option byte bit 1 is set,
//   this packet is sent automatically as measurements are taken.
//

void tsip_5A (void)
{
    uint8  pid                  = tsip_uint8();
    uint8  prn                  = tsip_uint8();
    float  sample_length        = tsip_float();
    float  signal_level         = tsip_float();
    float  code_phase           = tsip_float();
    float  doppler              = tsip_float();
    double time_of_measurement  = tsip_double();

    Lprintf("0x%.2X: %d prn, %g len, %g sig, %g code, %g doppler, %lg time_of\n",
            pid,
            prn,
            sample_length,
            signal_level,
            code_phase,
            doppler,
            time_of_measurement);
}

//
// A.9.31 Report Packet 0x5B Satellite Ephemeris Status
// This packet is sent in response to command packet 0x3B and when a new ephemeris
// (based on IODE) is received if enabled with packet 0x8E-A5.
//

void tsip_5B (void)
{
    uint8  pid                  = tsip_uint8();
    uint8  prn                  = tsip_uint8();
    float  time_of_collection   = tsip_float();
    uint8  heath                = tsip_uint8();
    uint8  iode                 = tsip_uint8();
    float  toe                  = tsip_float();
    uint8  fit_interval_flag    = tsip_uint8();
    float  ura                  = tsip_float();

    Lprintf("0x%.2X: %d prn, %g time, %d heath, %d iode, %g toe, %d fit, %g ura\n",
            pid,
            prn,
            time_of_collection,
            heath,
            iode,
            toe,
            fit_interval_flag,
            ura);
}

//
// A.9.34 Report Packet 0x6D Satellite Selection List
//  - the GPS receiver sends this packet in response to packet 0x24
//  - if enabled with packet 8E-A5, the receiver will send this packet
//    whenever the selection list is updated
//
void tsip_6D (void)
{
    uint8  pid  = tsip_uint8();
    uint8  fix  = tsip_uint8();
    float  pdop = tsip_float();
    float  hdop = tsip_float();
    float  vdop = tsip_float();
    float  tdop = tsip_float();
    char buf[1000];
    int count;

    strcpy(buf, "");
    count = (fix & 0xF0) >> 4;
    if (count > 0 && count <= MAX_CHAN) {
        int i;
        sint8 prn[MAX_CHAN];
        char *p = buf;

        for (i = 0; i < count; i += 1) {
            prn[i] = tsip_sint8();
            p += sprintf(p, "%d ", prn[i]);
        }
    }
    Lprintf("0x%.2X: %d dim (134=023D), %d mode (0=auto), %g %g %g %g PHVTdop, %d nsv [ %s]\n",
            pid,
            (fix & 0x07),       // fix dimenision
            (fix & 0x08) >> 3,  // fix mode
            pdop, hdop, vdop, tdop,
            count,              // number of sv in fix
            buf);
}

//
// A.10.17 Report Packet 0x8F-41 Stored Manufacturing Operating Parameters.
//  - this packet is sent in response to packet 0x8E-41
//
void tsip_8F_41 (void)
{
    uint8  pid                             = tsip_uint8();
    uint8  sub                             = tsip_uint8();
    sint16 board_serial_number_prefix      = tsip_sint16();
    uint32 board_serial_number             = tsip_uint32();
    uint8  year_of_build                   = tsip_uint8();
    uint8  month_of_build                  = tsip_uint8();
    uint8  day_of_build                    = tsip_uint8();
    uint8  hour_of_build                   = tsip_uint8();
    float  oscillator_offset               = tsip_float();
    uint16 test_code_identification_number = tsip_uint16();

    Lprintf("0x%.2X-%.2X: BOARD: %d+%lu %.4u-%.2u-%.2u:%.2u %.8le offset, %u id\n",
            pid, sub,
            board_serial_number_prefix,
            board_serial_number,
            Y2K_ADJUST(year_of_build),
            month_of_build,
            day_of_build,
            hour_of_build,
            oscillator_offset,
            test_code_identification_number);
}

//
// A.10.18 Report Packet 0x8F-42 Stored Production Parameters.
//  - this packet is sent in response to packet 0x8E-42
//
void tsip_8F_42 (void)
{
    uint8  pid                           = tsip_uint8();
    uint8  sub                           = tsip_uint8();
    uint8  production_options_prefix     = tsip_uint8();
    uint8  production_number_extension   = tsip_uint8();
    uint16 case_serial_number_prefix     = tsip_uint16();
    uint32 case_serial_number            = tsip_uint32();
    uint32 production_number             = tsip_uint32();
    uint16 reserved1                     = tsip_uint16();
    uint16 machine_identification_number = tsip_uint16();
    uint16 reserved2                     = tsip_uint16();

    Lprintf("0x%.2X-%.2X: PROD: %u+%u CASE: %u+%lu PROD: %lu MACH: %u\n",
            pid, sub,
            production_options_prefix,
            production_number_extension,
            case_serial_number_prefix,
            case_serial_number,
            production_number,
            machine_identification_number);
}

//
// A.10.22 Report Packet 0x8F-A0 DAC Value.
//
void tsip_8F_A0 (void)
{
    uint8  pid             = tsip_uint8();
    uint8  sub             = tsip_uint8();
    uint32 dac_value       = tsip_uint32();
    float  dac_voltage     = tsip_float();
    uint8  dac_resolution  = tsip_uint8();
    uint8  dac_data_format = tsip_uint8();
    float  min_dac_voltage = tsip_float();
    float  max_dac_voltage = tsip_float();

    Lprintf("0x%.2X-%.2X: 0x%.5lx DAC, %.6lf V, %u bits, %u fmt, %g Vmin, %g Vmax\n",
            pid, sub,
            dac_value,
            dac_voltage,
            dac_resolution,
            dac_data_format,
            min_dac_voltage,
            max_dac_voltage);
}

//
// A.10.23 Report Packet 0x8F-A1 10 MHz Sense.
//
void tsip_8F_A1 (void)
{
    uint8  pid           = tsip_uint8();
    uint8  sub           = tsip_uint8();
    uint8  ten_mhz_sense = tsip_uint8();

    Lprintf("0x%.2X-%.2X: %u 10 MHz sense (0=rise)\n", pid, sub, ten_mhz_sense);
}

//
// A.10.24 Report Packet 0x8F-A2 UTC/GPS Timing Mode.
//
void tsip_8F_A2 (void)
{
    uint8  pid          = tsip_uint8();
    uint8  sub          = tsip_uint8();
    uint8  utc_gps_time = tsip_uint8();

    Lprintf("0x%.2X-%.2X: %u utc_gps (3=UTC)\n", pid, sub, utc_gps_time);
}

//
// A.10.9 Packet 0x8E-A3 Issue Disciplining Command.
// - responds with packet 0x8F-A3 in the same format.
//
void tsip_8F_A3 (void)
{
    uint8  pid                  = tsip_uint8();
    uint8  sub                  = tsip_uint8();
    uint8  disciplining_command = tsip_uint8();

    Lprintf("0x%.2X-%.2X: %d disciplining\n", pid, sub, disciplining_command);
}

//
// A.10.27 Report Packet 0x8F-A7 Individual Satellite Solutions.
//
void tsip_8F_A7 (int size)
{
    uint8  pid    = tsip_uint8();
    uint8  sub    = tsip_uint8();
    uint8  format = tsip_uint8();
    char buf[1000];

#define NS 1e9
    strcpy(buf, "");
    if (format == 0) {
        uint32 time_of_fix     = tsip_uint32();
        float  clock_bias      = tsip_float();
        float  clock_bias_rate = tsip_float();
        int count, i;
        char *p = buf;

        count = (size - 15) / 5;
        if (count != 8) {
            Lprintf("0x%.2X-%.2X:%d size %d, count %d ??\n",
                    pid, sub, format, size, count);
        }
        for (i = 0; i < count; i += 1) {
            uint8 sid[MAX_CHAN];
            float clock_bias[MAX_CHAN];

            sid[i] = tsip_uint8();
            clock_bias[i] = tsip_float();
            // Use nanosecond units for clock bias
            p += sprintf(p, "%d:%.6f ", sid[i], clock_bias[i] * NS);
        }

        Lprintf("0x%.2X-%.2X:%d size %d, %.6f ns, %.6e rate, %d nsv [ %s]\n",
                pid, sub, format, size,
                clock_bias * NS, clock_bias_rate, count, buf);
    } else {
        Lprintf("0x%.2X-%.2X:%d size %d ??\n",
                pid, sub, format, size);
    }
}

//
// A.10.29 Report Packet 0x8F-A9 Self-Survey Parameters.
//
void tsip_8F_A9 (void)
{
    uint8  pid                = tsip_uint8();
    uint8  sub                = tsip_uint8();
    uint8  self_survey_enable = tsip_uint8();
    uint8  position_save_flag = tsip_uint8();
    uint32 self_survey_length = tsip_uint32();
    uint32 reserved           = tsip_uint32();

    Lprintf("0x%.2X-%.2X: %u self survey enable, %u position save, %lu length\n",
            pid, sub,
            self_survey_enable,
            position_save_flag,
            self_survey_length);
}

//
// A.10.30 Report Packet 0x8F-AB Primary Timing Packet.
//
void tsip_8F_AB (void)
{
    uint8  pid          = tsip_uint8();
    uint8  sub          = tsip_uint8();
    uint32 time_of_week = tsip_uint32();
    uint16 week_number  = tsip_uint16();
    sint16 utc_offset   = tsip_uint16();
    uint8  timing_flag  = tsip_uint8();
    uint8  seconds      = tsip_uint8();
    uint8  minutes      = tsip_uint8();
    uint8  hours        = tsip_uint8();
    uint8  day_of_month = tsip_uint8();
    uint8  month        = tsip_uint8();
    uint16 year         = tsip_uint16();

    Lprintf("0x%.2X-%.2X: %lu tow, %u wn, %u ls, 0x%.2X, %.4u-%.2u-%.2u %.2u:%.2u:%.2u\n",
            pid, sub,
            time_of_week, week_number, utc_offset, timing_flag,
            year, month, day_of_month,
            hours, minutes, seconds);

    // Use TOW as a warning for lost packets.

    if (DO_CHECKS) {
        static long prev_tow = -1;
        if (prev_tow != -1) {
            if (time_of_week == (uint32) (prev_tow + 1)) {
                ;
            } else if (time_of_week == 0 && prev_tow == (86400 * 7 - 1)) {
                ;
            } else {
                fprintf(stderr, "** TOW sequence error? (%ld != %ld+1)\n",
                        time_of_week, prev_tow);
            }
        }
        prev_tow = time_of_week;
    }
}

// Return double as float as binary long.
long Float32 (double d)
{
    float f = (float) d;
    long i = *(long *)&f;
    return i;
}
#define Upper32(d) ( ((long *)&(d))[1] )
#define Lower32(d) ( ((long *)&(d))[0] )

//
// A.10.31 Report Packet 0x8F-AC Secondary Timing Packet.
//
void tsip_8F_AC (void)
{
    uint8  pid                   = tsip_uint8();
    uint8  sub                   = tsip_uint8();
    uint8  receiver_mode         = tsip_uint8();
    uint8  disciplining_mode     = tsip_uint8();
    uint8  self_survey_progress  = tsip_uint8();
    uint32 holdover_duration     = tsip_uint32();
    uint16 critical_alarms       = tsip_uint16();
    uint16 minor_alarms          = tsip_uint16();
    uint8  gps_decoding_status   = tsip_uint8();
    uint8  disciplining_activity = tsip_uint8();
    uint8  spare_status1         = tsip_uint8();
    uint8  spare_status2         = tsip_uint8();
    float  pps_offset_ns         = tsip_float();
    float  ten_mhz_offset_ppb    = tsip_float();
    uint32 dac_value             = tsip_uint32();
    float  dac_voltage           = tsip_float();
    float  temperature           = tsip_float();
    double latitude              = tsip_double();
    double longitude             = tsip_double();
    double altitude              = tsip_double();
    double spare                 = tsip_double();

    Lprintf("0x%.2X-%.2X:a %u rcv, %u dsc, %u pct, %lu hold, %.4X crit, %.4X min, %u gps, %u act\n",
            pid, sub,
            receiver_mode,
            disciplining_mode,
            self_survey_progress,
            holdover_duration,
            critical_alarms,
            minor_alarms,
            gps_decoding_status,
            disciplining_activity);

    Lprintf("0x%.2X-%.2X:b %.6lf C, %.6lf ns, %.6lf ppb, 0x%.5lx DAC, %.6lf V\n",
            pid, sub,
            temperature,
            pps_offset_ns,
            ten_mhz_offset_ppb,
            dac_value,
            dac_voltage);

    Lprintf("0x%.2X-%.2X:c %.9lf lat, %.9lf lon, %.6lf alt\n",
            pid, sub,
            DEG(latitude),
            DEG(longitude),
            altitude);

    if (opt.hex) {
        Lprintf("0x%.2X-%.2X:d 0x%.8lx,%.8lx %.8lx lat, 0x%.8lx,%.8lx %.8lx lon, 0x%.8lx,%.8lx %.8lx alt\n",
                pid, sub,
                Upper32(latitude), Lower32(latitude), Float32(latitude),
                Upper32(longitude), Lower32(longitude), Float32(longitude),
                Upper32(altitude), Lower32(altitude), Float32(altitude));
    }

    // Rough check for temperature glitches.

    if (DO_CHECKS) {
        static float prev_temp = -1;
        if (prev_temp != -1) {
            double fabs (double);
            float dt = temperature - prev_temp;
            if (fabs(dt) > 0.5) {
                fprintf(stderr, "** temperature glitch? (%lg = %lg - %lg)\n",
                        dt, temperature, prev_temp);
            }
        }
        prev_temp = temperature;
    }

    // Check for end of data collection.

    stat.seconds += 1;
    if (opt.max_sec != 0) {
        // Echo progress by seconds remaining.
        fprintf(stderr, "\r%ld \r", stat.seconds - opt.max_sec);
        if (stat.seconds >= opt.max_sec) {
            quit();
        }
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
