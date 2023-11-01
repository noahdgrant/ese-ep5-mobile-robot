/*******************************************************************************
* Name: Joystick.h (interface)
* Author(s):Wyatt Richard
* With Code from Jason White
* Date: October 30, 2023
* Description: Joystick functions.
*******************************************************************************/

#ifndef DCMOTOR_H
#define	DCMOTOR_H

int read_event(int fd, struct js_event *event);

size_t get_axis_count(int fd);

size_t get_button_count(int fd);

struct axis_state {
    short x, y;
};

size_t get_axis_state(struct js_event *event, struct axis_state axes[3]);

#endif