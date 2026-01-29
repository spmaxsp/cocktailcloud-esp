
#ifndef SHIFT_REG_H
#define SHIFT_REG_H

/* -------------------------------------------------------------------------- */
/*                                SHIFTREGISTER                               */
/* -------------------------------------------------------------------------- */

void setup_shiftregister(void);
void shiftreg_clear(void);
void shiftreg_write(bool shiftreg_data[16]);

#endif // SHIFT_REG_H