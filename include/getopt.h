/****************************************************************************

  getopt.h - Read command line options

AUTHOR: Gregory Pietsch
CREATED Thu Jan 09 22:37:00 1997

DESCRIPTION:

The getopt() function parses the command line arguments.  Its arguments argc
and argv are the argument count and array as passed to the main() function
on program invocation.  The argument optstring is a list of available option
characters.  If such a character is followed by a colon (`:'), the option
takes an argument, which is placed in optarg.  If such a character is
followed by two colons, the option takes an optional argument, which is
placed in optarg.  If the option does not take an argument, optarg is NULL.

The external variable optind is the index of the next array element of argv
to be processed; it communicates from one call to the next which element to
process.

The getopt_long() function works like getopt() except that it also accepts
long options started by two dashes `--'.  If these take values, it is either
in the form

--arg=value

or

--arg value

It takes the additional arguments longopts which is a pointer to the first
element of an array of type GETOPT_LONG_OPTION_T, defined below.  The last
element of the array has to be filled with NULL for the name field.

The longind pointer points to the index of the current long option relative
to longopts if it is non-NULL.

The getopt() function returns the option character if the option was found
successfully, `:' if there was a missing parameter for one of the options,
`?' for an unknown option character, and EOF for the end of the option list.

The getopt_long() function's return value is described below.

The function getopt_long_only() is identical to getopt_long(), except that a
plus sign `+' can introduce long options as well as `--'.

Describe how to deal with options that follow non-option ARGV-elements.

If the caller did not specify anything, the default is REQUIRE_ORDER if the
environment variable POSIXLY_CORRECT is defined, PERMUTE otherwise.

REQUIRE_ORDER means don't recognize them as options; stop option processing
when the first non-option is seen.  This is what Unix does.  This mode of
operation is selected by either setting the environment variable
POSIXLY_CORRECT, or using `+' as the first character of the optstring
parameter.

PERMUTE is the default.  We permute the contents of ARGV as we scan, so that
eventually all the non-options are at the end.  This allows options to be
given in any order, even with programs that were not written to expect this.

RETURN_IN_ORDER is an option available to programs that were written to
expect options and other ARGV-elements in any order and that care about the
ordering of the two.  We describe each non-option ARGV-element as if it were
the argument of an option with character code 1.  Using `-' as the first
character of the optstring parameter selects this mode of operation.

The special argument `--' forces an end of option-scanning regardless of the
value of `ordering'.  In the case of RETURN_IN_ORDER, only `--' can cause
getopt() and friends to return EOF with optind != argc.

COPYRIGHT NOTICE AND DISCLAIMER:

Copyright (C) 1997 Gregory Pietsch

This file and the accompanying getopt.c implementation file are hereby 
placed in the public domain without restrictions.  Just give the author 
credit, don't claim you wrote it or prevent anyone else from using it.

Gregory Pietsch's current e-mail address:
gpietsch@comcast.net
****************************************************************************/

#ifndef GETOPT_H
#define GETOPT_H

/* include files needed by this include file */

/* macros defined by this include file */
#define no_argument          0
#define required_argument    1
#define optional_argument    2

#define getopt_none			 0
#define getopt_int			 1
#define getopt_double		 2
#define getopt_string		 3

#define getopt_unspecified	 0
#define	getopt_specified	 1

/* types defined by this include file */

/* struct option: The type of long option */
struct option
{
    char *name;                   /* the name of the long option */
    int has_arg;                  /* one of the above macros */
    int *flag;                    /* determines if getopt_long() returns a
                                   * value for a long option; if it is
                                   * non-NULL, 0 is returned as a function
                                   * value and the value of val is stored in
                                   * the area pointed to by flag.  Otherwise,
                                   * val is returned. */
    int val;                      /* determines the value to return if flag is
                                   * NULL. */
	char *help;					  /* help display on the line of the option */
	char *param_name;			  /* name to use for the parameter */
	int type;					  /* expected type for the parameter */
	void* value;				  /* pointer to a location where to store the parameter value */
	int* specified;				  /* pointer to a locatino where to store 0 if option wasn't specified on command line, 1 if it was*/
};

#ifdef __cplusplus
extern "C"
{
#endif

    /* externally-defined variables */
    extern char*	optarg;
    extern int		optind;
    extern int		opterr;
    extern int		optopt;

	extern int		opterrorcodes[256]; /* Used by getopt_parse, Contains a list of all the error code encountered while parsing the options*/
	extern char		opterrorshorts[256]; /* Used by getopt_parse, Contains a list of all the optinos with errors encountered while parsing the options*/

    /* function prototypes */
    int getopt (int argc, char **argv, const char *optstring);
    int getopt_long (int argc, char **argv, const char *shortopts, const struct option *longopts, int *longind);
    int getopt_long_only (int argc, char **argv, const char *shortopts, const struct option *longopts, int *longind);

	void getopt_usage(char* progname, char *short_desc, char *pre_options, char *post_options, const struct option *longopts);
	int getopt_parse(int argc, char **argv, struct option *longopts, char *short_desc, char *pre_options, char *post_options);

#ifdef __cplusplus
};

#endif

#endif /* GETOPT_H */
