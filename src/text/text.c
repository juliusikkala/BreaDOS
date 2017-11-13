#include "text.h"
#include "types.h"
#include <stdarg.h>
#include <limits.h>

struct text_device* default_text_device = NULL;

void set_default_text_device(struct text_device* device)
{
    default_text_device = device;
}

struct text_device* get_default_text_device()
{
    return default_text_device;
}

static int num_len(uint64_t u, int base)
{
    size_t len = 1;
    while((u /= base)) len++;
    return len;
}

/* Adds padding, prefix and postfix to content*/
static int padding(
    void* userdata,
    void (*output_char)(char, void*),
    const char* content,
    size_t content_max_len,
    const char* prefix,
    const char* postfix,
    char pad_char,
    bool pad_after_prefix,
    bool left_justify,
    ssize_t pad_width
){
    int written_chars = 0;

    if(pad_after_prefix && prefix)
    {
        while(*prefix)
        {
            output_char(*prefix++, userdata);
            written_chars++;
            pad_width--;
        }
    }

    if(!left_justify)
    {
        ssize_t content_len =
            strnlen(content, content_max_len)
            + (!pad_after_prefix ? strlen(prefix) : 0)
            + strlen(postfix);
        if(pad_width > content_len)
        {
            pad_width -= content_len;
            while(pad_width--)
            {
                output_char(pad_char, userdata);
                written_chars++;
            }
        }
    }

    if(!pad_after_prefix && prefix)
    {
        while(*prefix)
        {
            output_char(*prefix++, userdata);
            written_chars++;
            pad_width--;
        }
    }

    if(content)
    {
        while(content_max_len-- && *content)
        {
            output_char(*content++, userdata);
            written_chars++;
            pad_width--;
        }
    }

    if(postfix)
    {
        while(*postfix)
        {
            output_char(*postfix++, userdata);
            written_chars++;
            pad_width--;
        }
    }

    if(left_justify)
    {
        while(pad_width--) 
        {
            output_char(pad_char, userdata);
            written_chars++;
        }
    }

    return written_chars;
}

static int generic_putu(
    void* userdata,
    void (*output_char)(char, void*),
    uint64_t u,
    int base,
    bool lowercase,
    bool negative,
    bool left_justify,
    bool force_sign,
    bool insert_space_for_positive,
    bool force_decimal_point_or_prefix,
    bool zero_pad,
    size_t pad_width
){
    char prefix[4] = {0};
    char content[21] = {0};
    char* prefix_tmp = prefix;
    const char* postfix = NULL;

    if(base > 36 || base < 2) return 0;

    /* Add sign to prefix*/
    if(negative) *prefix_tmp++ = '-';
    else if(force_sign) *prefix_tmp++ = '+';
    else if(insert_space_for_positive) *prefix_tmp++ = ' ';

    if(force_decimal_point_or_prefix)
    {
        if(base == 10) postfix = ".";
        else if(base == 8) *prefix_tmp++ = '0';
        else if(base == 16)
        {
            *prefix_tmp++ = '0';
            *prefix_tmp++ = lowercase ? 'x' : 'X';
        }
        else if(base == 2)
        {
            *prefix_tmp++ = '0';
            *prefix_tmp++ = 'b';
        }
    }

    ultostr(u, content, base);
    if(lowercase) tolower(content);

    return padding(
        userdata,
        output_char,
        content,
        sizeof(content),
        prefix[0] == 0 ? NULL : prefix,
        postfix,
        !left_justify && zero_pad ? '0' : ' ',
        zero_pad,
        left_justify,
        pad_width
    );
}

static int generic_puts(
    void* userdata,
    void (*output_char)(char, void*),
    const char* s,
    size_t max_chars,
    bool left_justify,
    size_t pad_width
){
    return padding(
        userdata,
        output_char,
        s ? s : "(null)",
        max_chars,
        NULL, NULL,
        ' ', false, left_justify, pad_width
    );
}

static int generic_putp(
    void* userdata,
    void (*output_char)(char, void*),
    void* p,
    bool left_justify,
    size_t pad_width
){
    char content[17] = {
        '0', '0', '0', '0',
        '0', '0', '0', '0',
        '0', '0', '0', '0',
        '0', '0', '0', '0',
        0
    };

    ultostr((uint64_t)p, content + 16 - num_len((uint64_t)p, 16), 16);

    return padding(
        userdata,
        output_char,
        content,
        sizeof(content),
        "0x", NULL,
        ' ', false, left_justify, pad_width
    );
}

static int generic_printf(
    void* userdata,
    void (*output_char)(char, void*),
    const char* format,
    va_list arg
){
    int written_chars = 0;
    char c;
    while((c = *format++))
    {
        bool left_justify = false; /* - */
        bool force_sign = false; /* + */
        bool insert_space_for_positive = false; /* (space) */
        bool force_decimal_point_or_prefix = false; /* # */
        bool zero_pad = false; /* 0 */
        size_t pad_width = 0;
        ssize_t precision = -1;
        size_t length_mask = 0xFFFFFFFF;
        bool finished = false;

        if(c != '%')
        {
            output_char(c, userdata);
            written_chars++;
            continue;
        }

        while(!finished && *format)
        {
            const char* endptr = format + 1;
            int64_t d = 0;
            void* p = NULL;
            char c = *format;
            switch(c)
            {
            case '-':
                left_justify = true;
                break;
            case '+':
                force_sign = true;
                break;
            case ' ':
                insert_space_for_positive = true;
                break;
            case '#':
                force_decimal_point_or_prefix = true;
                break;
            case '0':
                zero_pad = true;
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                pad_width = strtoul(format, (char**)&endptr, 10);
                break;
            case '*':
                pad_width = va_arg(arg, size_t);
                break;
            case '.':
                format++;
                if(*format == '*')
                {
                    precision = va_arg(arg, size_t);
                }
                else
                {
                    precision = strtoul(format, (char**)&endptr, 10);
                }
                break;
            case 'h':
                length_mask = 0xFFFF;
                break;
            case 'l':
                length_mask = 0xFFFFFFFFFFFFFFFF;
                break;
            case '%':
                output_char('%', userdata);
                written_chars++;
                finished = true;
                break;
            case 'c':
                output_char(va_arg(arg, int), userdata);
                written_chars++;
                finished = true;
                break;
            case 's':
                written_chars += generic_puts(
                    userdata,
                    output_char,
                    va_arg(arg, const char*),
                    precision >= 0 ? (size_t)precision : ULONG_MAX,
                    left_justify, pad_width
                );
                finished = true;
                break;
            case 'n':
                *(va_arg(arg, int*)) = written_chars;
                finished = true;
                break;
            case 'p':
                p = va_arg(arg, void*);
                written_chars += generic_putp(
                    userdata, output_char, p, left_justify, pad_width
                );
                finished = true;
                break;
            case 'd':
            case 'i':
            case 'u':
            case 'o':
            case 'x':
            case 'X':
            case 'b':
                d = va_arg(arg, int64_t) & length_mask;
                written_chars += generic_putu(
                    userdata,
                    output_char,
                    /* Number to print; must be unsigned */
                    (d < 0 && (c == 'd' || c == 'c') ? -d : d),
                    /* Base*/
                    (c == 'o' ? 8 :
                        (c == 'x' || c == 'X' ? 16 :
                            (c == 'b' ? 2 : 10))), 
                    /* Lowercase*/
                    c == 'x',
                    /* Sign */
                    (c == 'd' || c == 'c' ? d < 0 : false),
                    left_justify, force_sign, insert_space_for_positive,
                    force_decimal_point_or_prefix, zero_pad, pad_width
                );
                finished = true;
                break;
            default:
                finished = true;
                break;
            }
            format = endptr;
        }
    }
    return written_chars;
}

int printf_dev(struct text_device* device, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vprintf_dev(device, format, args);
    va_end(args);
    return ret;
}

int printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vprintf_dev(get_default_text_device(), format, args);
    va_end(args);
    return ret;
}

static void output_char_device(char c, struct text_device* device)
{
    device->driver->putc(device, c);
}

int vprintf_dev(struct text_device* device, const char* format, va_list arg)
{
    return generic_printf(
        device,
        (void (*)(char, void*))output_char_device,
        format,
        arg
    );
}

int vprintf(const char* format, va_list arg)
{
    return vprintf_dev(get_default_text_device(), format, arg);
}

int sprintf(char* s, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vsprintf(s, format, args);
    va_end(args);
    return ret;
}

static void output_char_string(char c, char** s)
{
    **s = c;
    (*s)++;
}

static void output_char_dummy(char c, void* s) { (void)c; (void)s; }

int vsprintf(char* s, const char* format, va_list arg)
{
    if(s == NULL)
    {
        return generic_printf(
            NULL,
            (void (*)(char, void*))output_char_dummy,
            format,
            arg
        ) + 1;
    }
    int len = generic_printf(
        &s,
        (void (*)(char, void*))output_char_string,
        format,
        arg
    );
    *s = 0;
    return len + 1;
}

uint64_t strtoul(const char* str, char** endptr, int base)
{
    if(base > 36 || base < 2) return 0;
    unsigned long val = 0;

    for(char c = 0; (c = *str); str++)
    {
        // lowercase to uppercase
        if(c > 96) c-=32;
        // characters after numbers
        if(c > 64) c-=7;
        // ascii numbers to numbers
        c -= 48;
        if(c < 0 || c >= base) break;
        val = val*base + c;
    }

    *endptr = (char*)str;
    return val;
}

static char* ultostr_recursive(unsigned long u, char* str, int base)
{
    static const char* num_table = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if(u == 0) return str;
    str = ultostr_recursive(u / base, str, base);
    *str = num_table[u % base];
    return str + 1;
}

char* ultostr(unsigned long u, char* str, int base)
{
    if(u == 0)
    {
        str[0] = '0';
        str[1] = 0;
    }
    else *ultostr_recursive(u, str, base) = 0;

    return str;
}

char* tolower(char* str)
{
    for(char* tmp = str; *tmp; tmp++)
    {
        if(*tmp >= 'A' && *tmp <= 'Z')
        {
            tmp += 'a' - 'A';
        }
    }

    return str;
}

char* toupper(char* str)
{
    for(char* tmp = str; *tmp; tmp++)
    {
        if(*tmp >= 'a' && *tmp <= 'z')
        {
            tmp -= 'a' - 'A';
        }
    }

    return str;
}

size_t strlen(const char* str)
{
    if(str == NULL) return 0;
    const char* start = str;
    while(*str++);
    return str - start - 1;
}

size_t strnlen(const char* str, size_t max_len)
{
    if(str == NULL) return 0;
    const char* start = str;
    while(*str++ && max_len--);
    return str - start - 1;
}

char* strncpy(char* dst, const char* str, size_t num)
{
    char* tmp = dst;
    if(str) while(num-- && (*tmp++ = *str++));
    while(num--) *tmp++ = 0;
    return tmp;
}
