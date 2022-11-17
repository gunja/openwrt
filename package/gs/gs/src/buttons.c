#include "buttons.h"

static int buttons_fd;

static const char *const evval[3] = {
    "RELEASED",
    "PRESSED ",
    "REPEATED"
};

void (*callback0)(rk_t*, int);
void (*callback1)(rk_t*, int);
void (*callback2)(rk_t*, int);
void (*callback3)(rk_t*, int);

void buttons_init(rk_t* left, rk_t* right){
    int fd = -1;
    fd = open(INPUT_DEV, O_RDONLY);
    if (fd < 0)
    {
        printf("open buttons event failed.\n");
    } else {
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
    buttons_fd = fd;

    callback0 = left->btn_clbk_start;
    callback1 = left->btn_clbk_stop;
    callback2 = right->btn_clbk_start;
    callback3 = right->btn_clbk_stop;
    return 0;
}

int buttons_handler(rk_t* left, rk_t* right){
    int ret = -1;
    struct input_event ev;
    memset(&ev, 0, sizeof(struct input_event));

    ret = read( buttons_fd,
                &ev,
                sizeof(struct input_event));

    if (ret == (ssize_t)-1) {
        if (errno == EAGAIN || errno == EINTR) {
                return 0;
        }
    } else {
        if (ret != sizeof ev) {
            errno = EIO;
            return -1;
        }

        if (ev.type == EV_KEY && ev.value == 1) {
            switch (ev.code)
            {
                case BTN_0: // start 1
                    callback0(left, ev.code);
                    break;
                case BTN_1:// stop 1
                    callback1(left, ev.code);
                    break;
                case BTN_2: //start 2
                    callback2(right, ev.code);
                    break;
                case BTN_3: // stop 2
                    callback3(right, ev.code);
                    break;
                default: // non-keyboard event
                    break;
            }
        }
    }
    return 0;
}
