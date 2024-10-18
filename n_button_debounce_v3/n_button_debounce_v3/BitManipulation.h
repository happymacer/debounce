/****************************************************
 * BitManipulation.h								*
 *													*
 * Created: 26/09/2024								*
 *  Author: Happymacer								*
 *													*
 * There is no C implementation file				*													*
 ***************************************************/ 

#ifndef BITMANIPULATION_H
#define BITMANIPULATION_H

//defines
#define SET_BIT(byte, bit) (byte |= (1 << bit))
#define CLEAR_BIT(byte, bit) (byte &= ~(1 << bit))
#define TOGGLE_BIT(byte, bit) (byte ^= (1 << bit))

#endif /* BITMANIPULATION_H */