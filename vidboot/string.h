/*
 * string handling routines
 */
#ifndef __STRING_H__

unsigned strlen( char *str );
unsigned strcat( char *, const char *);
unsigned strcpy( char *dst, const char *src );
unsigned strncpy( char *dst, const char *src, unsigned nbytes );
unsigned bcopy( void *src, void *dst, unsigned nbytes );
void bzero( void *, int );

#define __STRING_H__
#endif