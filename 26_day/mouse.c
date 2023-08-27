#include "mouse.h"
#include "fifo.h"
#include "int.h"
#include "io.h"
#include "keyboard.h"

struct FIFO32 *mousefifo;
int mousedata0;

void enable_mouse(struct FIFO32 *fifo, int data0, struct MouseDec *mdec) {
  /* 将FIFO缓冲区的信息保存到全局变量里 */
  mousefifo = fifo;
  mousedata0 = data0;
  /* 鼠标有效 */
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
  /* 顺利的话，ACK(0xfa)会被发送*/
  mdec->phase = 0; /* 等待鼠标的0xfa的阶段*/
}

int mouse_decode(struct MouseDec *mdec, unsigned char dat) {
  if (mdec->phase == 0) {
    // 等待鼠标的0xfa状态
    if (dat == 0xfa) {
      mdec->phase = 1;
    }
    return 0;
  } else if (mdec->phase == 1) {
    // 等待鼠标的第一个字节
    if ((dat & 0xc8) == 0x08) {
      // 如果第一个字节正确
      mdec->buf[0] = dat;
      mdec->phase = 2;
    }
    return 0;
  } else if (mdec->phase == 2) {
    // 等待鼠标的第二个字节
    mdec->buf[1] = dat;
    mdec->phase = 3;
    return 0;
  } else if (mdec->phase == 3) {
    // 等待鼠标的第三个字节
    mdec->buf[2] = dat;
    mdec->phase = 1;

    mdec->btn = mdec->buf[0] & 0x07;
    mdec->x = mdec->buf[1];
    mdec->y = mdec->buf[2];

    if ((mdec->buf[0] & 0x10) != 0) {
      mdec->x |= 0xffffff00;
    }

    if ((mdec->buf[0] & 0x20) != 0) {
      mdec->y |= 0xffffff00;
    }

    mdec->y = -mdec->y; // 鼠标的y方向与画面符号相反

    return 1;
  }
  return -1;
}

/* 来自PS/2鼠标的中断 */
void int_handler2c(int *esp) {
  io_out8(PIC1_OCW2, 0x64); // 通知PIC1 IRQ-12的受理已经完成
  io_out8(PIC0_OCW2, 0x62); // 通知PIC0 IRQ-02的受理已经完成
  int data = io_in8(PORT_KEYDAT);

  fifo32_put(mousefifo, data + mousedata0);
}