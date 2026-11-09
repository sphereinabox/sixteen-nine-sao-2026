all : flash

TARGET:=anim_bounce

ADDITIONAL_C_FILES:=anim_plasma.c sao_anim_main.c anim_conways.c anim_textscroll.c textscroll_font.c ../../sao-ch32-is31/is31.c

EXTRA_CFLAGS:=-I../../include
TARGET_MCU?=CH32V003
include ~/git/ch32fun/ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean

