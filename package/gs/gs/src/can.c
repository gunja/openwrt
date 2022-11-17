#include "can.h"

#define FILENAME_MAX_SIZE   64

static int CAN_socket_init(char* interface);
static int CAN_send(can_t* self, float volume, float price, float totalPrice);
static int CAN_send_half(can_t* self, float volume, float totalPrice);

int CAN_init(int idx, can_t* can) {

    char filename[FILENAME_MAX_SIZE];

    // busNumber
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_CAN_BUS_NUMBER, idx);
    memset(can->config_filename_bus_number, 0, sizeof(can->config_filename_bus_number));
    strcpy(can->config_filename_bus_number, filename);
    int ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    can->busNumber = ret;

    sprintf(can->bus, "can%d", can->busNumber);

    can->fd = CAN_socket_init(can->bus);

    if(can->fd == -1) {
        return -1;
    }

    // deviceBaseAddress
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_CAN_DEVICE_ADDRESS, idx);
    memset(can->config_filename_device_address, 0, sizeof(can->config_filename_device_address));
    strcpy(can->config_filename_device_address, filename);
    ret = parse_integer_config(filename);

    if(ret == -1) {
        return -1;
    }
    can->deviceAddress = ret;

    can->transmit = CAN_send;
    can->transmit_half = CAN_send_half;
}

static int CAN_socket_init(char* interface) {
    int sc;
    int flags;
    struct ifreq ifr;
    struct sockaddr_can caddr;
    socklen_t caddrlen = sizeof(caddr);

    memset(&caddr, 0, sizeof(caddr));
    strcpy(ifr.ifr_name, interface);

    if ((sc = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        fprintf(stderr, "CAN socket error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    flags = fcntl(sc, F_GETFL);
    fcntl(sc, F_SETFL, flags | O_NONBLOCK);

    caddr.can_family = AF_CAN;
    if (ioctl(sc, SIOCGIFINDEX, &ifr) < 0) {
        fprintf(stderr, "CAN setup error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    caddr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sc, (struct sockaddr *)&caddr, caddrlen) < 0) {
        fprintf(stderr, "CAN bind error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sc;
}

static int CAN_send(can_t* self, float volume, float price, float totalPrice) {
    // prepare data
    char frame_data[8] = {0};
    char frame2_data[4] = {0};
    memset(frame_data, 0, sizeof(frame_data));
    memset(frame2_data, 0, sizeof(frame2_data));
    memcpy(frame_data+4, &volume, sizeof(volume));
    memcpy(frame_data, &totalPrice, sizeof(totalPrice));
    memcpy(frame2_data, &price, sizeof(price));

    // send frame for upper and middle rows data
    self->frame.can_id = self->deviceAddress;
    self->frame.can_dlc = sizeof (frame_data);

    memcpy(&self->frame.data, frame_data, sizeof(frame_data));
    // To-Do: handle case of write() hangs if no devices on the bus
    int res = write(self->fd, &self->frame, sizeof(self->frame));
    if (res != sizeof(self->frame)) {
        if (errno != EAGAIN)
            fprintf(stderr, "CAN write error: %s\n", strerror(errno));
    }

    // send frame for lower row data
    self->frame.can_id = self->deviceAddress + 1;
    self->frame.can_dlc = sizeof (frame2_data);
    memcpy(&self->frame.data, frame2_data, sizeof(frame2_data));

    if (write(self->fd, &self->frame, sizeof(self->frame)) != sizeof(self->frame))
        if (errno != EAGAIN)
            fprintf(stderr, "CAN write error: %s\n", strerror(errno));

}

static int CAN_send_half(can_t* self, float volume, float totalPrice) {
    // prepare data
    char frame_data[8] = {0};
    memset(frame_data, 0, sizeof(frame_data));
    memcpy(frame_data+4, &volume, sizeof(volume));
    memcpy(frame_data, &totalPrice, sizeof(totalPrice));

    // send frame for upper and middle rows data
    self->frame.can_id = self->deviceAddress;
    self->frame.can_dlc = sizeof (frame_data);

    memcpy(&self->frame.data, frame_data, sizeof(frame_data));
    // To-Do: handle case of write() hangs if no devices on the bus
    int res = write(self->fd, &self->frame, sizeof(self->frame));
    if (res != sizeof(self->frame)) {
        if (errno != EAGAIN)
            fprintf(stderr, "CAN write error: %s\n", strerror(errno));
    }

}

