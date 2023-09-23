#include <stdint.h>
#ifndef ICONS_H
#define ICONS_H

typedef struct {
     const uint8_t *data;
     uint16_t width;
     uint16_t height;
     uint8_t dataSize;
} tImage;

extern const tImage heatIcon;
extern const tImage coolIcon;
extern const tImage windFlow;
extern const tImage noWindFlow;
extern const tImage celciusIcon;


#endif // ICONS_H
