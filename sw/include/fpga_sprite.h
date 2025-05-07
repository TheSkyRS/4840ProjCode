/**
 * @file fpga_sprite.h
 * @brief FPGA Sprite Engine Kernel Driver Interface Header
 */

#ifndef FPGA_SPRITE_H
#define FPGA_SPRITE_H

#include <linux/ioctl.h>

/* IOCTL command definitions */
#define FPGA_SPRITE_MAGIC 'f'

/* Sprite attribute structure - matches hardware layout */
typedef struct {
    union {
        struct {
            unsigned int frame_id : 8;     /* Frame ID [7:0] */
            unsigned int pos_col : 10;     /* X coordinate [17:8] */
            unsigned int pos_row : 9;      /* Y coordinate [26:18] */
            unsigned int reserved : 3;     /* Reserved bits [29:27] */
            unsigned int flip : 1;         /* Horizontal flip [30] */
            unsigned int enable : 1;       /* Enable/Display [31] */
        } bits;
        unsigned int value;                /* Complete 32-bit value */
    };
} fpga_sprite_attr_t;

/* Control register bit definitions */
typedef struct {
    union {
        struct {
            unsigned int enable_display : 1;    /* Enable display */
            unsigned int enable_sprites : 1;    /* Enable sprite display */
            unsigned int reset_display : 1;     /* Reset display controller */
            unsigned int enable_collision : 1;  /* Enable collision detection */
            unsigned int active_tilemap : 4;    /* Active tilemap ID (0-15) */
            unsigned int audio_id : 8;          /* Audio ID to play */
            unsigned int reserved : 16;         /* Reserved bits */
        } bits;
        unsigned int value;                     /* Complete register value */
    };
} fpga_control_reg_t;

/* Status register bit definitions */
typedef struct {
    union {
        struct {
            unsigned int hcount : 12;           /* Horizontal counter */
            unsigned int vcount : 12;           /* Vertical counter */
            unsigned int collision_detected : 1; /* Collision detection flag */
            unsigned int vblank : 1;            /* Vertical blanking flag */
            unsigned int reserved : 6;          /* Reserved bits */
        } bits;
        unsigned int value;                     /* Complete register value */
    };
} fpga_status_reg_t;

/* Parameter structure passed to ioctl */
typedef struct {
    union {
        fpga_sprite_attr_t sprite;     /* Single sprite attribute */
        fpga_sprite_attr_t sprites[64]; /* Sprite attribute array */
        fpga_control_reg_t control;    /* Control register value */
        fpga_status_reg_t status;      /* Status register value */
        struct {
            unsigned int index;        /* Sprite index */
            fpga_sprite_attr_t attr;   /* Sprite attribute */
        } sprite_update;
        unsigned int tilemap_id;       /* Tilemap ID */
        unsigned int audio_id;         /* Audio ID */
        unsigned int enable_collision; /* Enable/Disable collision detection */
    };
    unsigned int count;               /* Count for batch operations */
} fpga_sprite_arg_t;

/* IOCTL commands */
#define FPGA_SPRITE_READ_STATUS _IOR(FPGA_SPRITE_MAGIC, 0, fpga_sprite_arg_t)
#define FPGA_SPRITE_WRITE_CONTROL _IOW(FPGA_SPRITE_MAGIC, 1, fpga_sprite_arg_t)
#define FPGA_SPRITE_READ_SPRITE _IOWR(FPGA_SPRITE_MAGIC, 2, fpga_sprite_arg_t)
#define FPGA_SPRITE_WRITE_SPRITE _IOW(FPGA_SPRITE_MAGIC, 3, fpga_sprite_arg_t)
#define FPGA_SPRITE_UPDATE_SPRITE_TABLE _IOW(FPGA_SPRITE_MAGIC, 4, fpga_sprite_arg_t)
#define FPGA_SPRITE_SET_TILEMAP _IOW(FPGA_SPRITE_MAGIC, 5, fpga_sprite_arg_t)
#define FPGA_SPRITE_SET_COLLISION _IOW(FPGA_SPRITE_MAGIC, 6, fpga_sprite_arg_t)
#define FPGA_SPRITE_PLAY_AUDIO _IOW(FPGA_SPRITE_MAGIC, 7, fpga_sprite_arg_t)
#define FPGA_SPRITE_WAIT_VBLANK _IO(FPGA_SPRITE_MAGIC, 8)

#endif /* FPGA_SPRITE_H */ 