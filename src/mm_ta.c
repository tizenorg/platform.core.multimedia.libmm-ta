/*
 * libmm-ta
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Jonghyuk Choi <jhchoi.choi@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _MM_TA_C_
#define _MM_TA_C_

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#include "mm_ta.h"

/* internal func. */
static void __free_cps(void);
static int __get_cp_index(char* name);

static void __free_accums(void);
static int __get_accum_index(char* name);

/* global var. */
gst_ta_checkpoint ** g_cps = NULL;
static int g_cp_index = 0;

gst_ta_accum_item ** g_accums = NULL;
static int g_accum_index = 0;
static int g_accum_longest_name = 0;
static unsigned long g_accum_first_time = 0xFFFFFFFF;
static int g_enable = 0;

#define MMTA_ENV_NAME	"MMTA_ENABLE"

void __set_enable_by_env ()
{
	char* env = getenv (MMTA_ENV_NAME);
	if (env != NULL && strcmp (env,"1") == 0)
		gst_ta_set_enable(1);
}

int gst_ta_init(void)
{
	__set_enable_by_env();

	if (g_enable == 0)
		return 0;

	if (g_accums)
		return 0;
	
	g_cps = (gst_ta_checkpoint **) malloc ( GST_TA_MAX_CHECKPOINT * sizeof(gst_ta_checkpoint *) );
	if(!g_cps)
		return -1;
	
	g_accums = (gst_ta_accum_item **) malloc ( GST_TA_MAX_CHECKPOINT * sizeof(gst_ta_accum_item *) );
	if(!g_accums)
		return -1;

	g_accum_first_time = 0xFFFFFFFF;
	
	return 0;
}

int gst_ta_release(void)
{
	if ( ! g_accums )
		return 0;
	
	__free_cps();
	__free_accums();

	g_accum_first_time = 0xFFFFFFFF;

	return 0;
}

void gst_ta_set_enable(int enable)
{
	printf("MMTA : setting enable to %d\n", enable);
	g_enable = enable;	
}

int gst_ta_get_numof_checkpoints()
{
	return g_cp_index;
}

char* gst_ta_fmt(const char* fmt, ...)
{
	static char ta_buf[512];
	va_list args;
	
	memset(ta_buf, '\0', 512);

	va_start(args, fmt);
	vsnprintf(ta_buf, 512, fmt, args);
	va_end(args);
	
	return ta_buf;
}


int gst_ta_add_checkpoint(char* name, int show, char* filename, int line)
{
	gst_ta_checkpoint * cp = NULL;
	struct timeval	t;
	
	if (!g_enable)
		return -1;

	if (!g_accums)
		return 0;

	if ( g_cp_index == GST_TA_MAX_CHECKPOINT )
		return -1;

	if ( !name )
		return -1;

	if( strlen ( name ) == 0 )
		return -1;

	cp = ( gst_ta_checkpoint * ) malloc( sizeof( gst_ta_checkpoint ) );
	if ( !cp )
		return -1;

	cp->name = strdup (name);
	if ( !cp->name )
	{
		free(cp);
		return -1;
	}

	if ( show )
		printf("[CHECK-POINT] %s...(%s:%d)\n", name, filename, line );

	gettimeofday( &t, NULL );
	cp->timestamp = t.tv_sec*1000000L + t.tv_usec;
#ifdef GST_EXT_TA_UNIT_MSEC	
	cp->timestamp = (cp->timestamp >= 1000)?cp->timestamp/1000:0;
#endif

	g_cps[g_cp_index] = cp;

	g_cp_index++;

	return 0;
}

void gst_ta_show_checkpoints(void)
{
	int i = 0;

	if (!g_accums)
		return;

	printf("BEGIN RESULT ============================\n");
	for ( i = 0; i < g_cp_index; i++ )
	{
		printf("[%d] %s : %ld us.\n", i, g_cps[i]->name, g_cps[i]->timestamp);
	}
	printf("END RESULT   ============================\n");
}

void gst_ta_show_diff(char* name1, char* name2)
{
	if (!g_accums)
		return ;


	printf("Time takes from [%s] to [%s] : %ld us.\n", name1, name2, gst_ta_get_diff(name1, name2));
}

unsigned long gst_ta_get_diff(char* name1, char* name2)
{
	int cp1, cp2;

	if (!g_accums)
		return 0;


	/* fail if bad param. */
	if ( !name1 || !name2 )
		return -1;

	/* fail if same. */
	if ( strcmp( name1, name2 ) == 0 )
		return -1;

	/* get index */
	if ( ( cp1 = __get_cp_index(name1) ) == -1 )
		return -1;

	if ( ( cp2 = __get_cp_index(name2) ) == -1 )
		return -1;

	/* NOTE :
	 * return value must be positive value.
	 * bcz the value of higher index of g_cps always higher than lower one. */
	return g_cps[cp2]->timestamp - g_cps[cp1]->timestamp;
	
}

static int __get_cp_index(char* name)
{
	int i;

	assert(name);
	
	/* find index */
	for ( i = 0; i < g_cp_index; i++ )
	{
		if ( strcmp( name, g_cps[i]->name ) == 0 )
			return i;
	}

	return -1;
}

static int __get_accum_index(char* name)
{
	int i;

	assert(name);
	
	/* find index */
	for ( i = 0; i < g_accum_index; i++ )
	{
		if ( strcmp( name, g_accums[i]->name ) == 0 )
			return i;
	}

	return -1;
}

static void __free_cps(void)
{
	int i = 0;

	if ( ! g_cps )
		return;

	for ( i = 0; i < g_cp_index; i++ )
	{
		if ( g_cps[i] )
		{
			if ( g_cps[i]->name )
				free ( g_cps[i]->name );

			free ( g_cps[i] );

			g_cps[i] = NULL;
		}
	}

	free ( g_cps );
	g_cps = NULL;

	g_cp_index = 0;
}

static void __free_accums(void)
{
	int i = 0;

	if ( ! g_accums )
		return;

	for ( i = 0; i < g_accum_index; i++ )
	{
		if ( g_accums[i] )
		{
			if ( g_accums[i]->name )
				free ( g_accums[i]->name );

			free ( g_accums[i] );

			g_accums[i] = NULL;
		}
	}

	g_accum_index = 0;
	g_accum_longest_name = 0;

	free ( g_accums );
	g_accums = NULL;
}


int gst_ta_accum_item_begin(char* name, int show, char* filename, int line)
{
	gst_ta_accum_item * accum = NULL;
	int index = 0;
	int name_len = 0;
	struct timeval t;

	if (!g_enable)
		return -1;

	if (!g_accums)
		return 0;



	if ( g_accum_index == GST_TA_MAX_ACCUM )
		return -1;

	if ( !name )
		return -1;

	name_len = strlen(name);
	if( name_len == 0 )
		return -1;

	/* if 'name' is new one. create new item. */
	if ( (index = __get_accum_index(name)) == -1 )
	{
		accum = ( gst_ta_accum_item * ) malloc( sizeof( gst_ta_accum_item ) );
		if ( !accum )
			return -1;

		/* clear first. */
		memset( accum, 0, sizeof (gst_ta_accum_item) );
		accum->elapsed_min = 0xFFFFFFFF;

		accum->name = strdup (name);
		if ( !accum->name )
		{
			free(accum);
			return -1;
		}

		/* add it to list. */
		g_accums[g_accum_index] = accum;
		g_accum_index++;

		if ( g_accum_longest_name < name_len )
			g_accum_longest_name = name_len;
		
	}
	else
	{
		accum = g_accums[index];
	}

	/* verify pairs of begin, end. */
	if (accum->on_estimate)
	{
		printf("[%s] is not 'end'ed!\n", accum->name);
		accum->num_unpair ++;
		return -1;
	}

	/* get timestamp */
	gettimeofday( &t, NULL );
	accum->timestamp = t.tv_sec*1000000L + t.tv_usec;
#ifdef GST_EXT_TA_UNIT_MSEC	
	accum->timestamp = ( accum->timestamp >= 1000) ? accum->timestamp/1000 : 0;
#endif	
	accum->on_estimate = 1;

	if ( accum->first_start == 0 )  {	/* assum that timestamp never could be zero.*/
		accum->first_start = accum->timestamp;

		if ( g_accum_first_time > accum->first_start )
			g_accum_first_time = accum->first_start ;
	}
		
	if ( show )
		printf("[ACCUM BEGIN] %s : %ld ---(%s:%d)\n", name, accum->timestamp, filename, line );

	accum->num_calls++;

	return 0;
}

int gst_ta_accum_item_end(char* name, int show, char* filename, int line)
{
	gst_ta_accum_item * accum = NULL;
	unsigned int tval = 0;
	int index = 0;
	struct timeval	t;

	if (!g_enable)
		return -1;

	if (!g_accums)
		return 0;


	/* get time first for more accuracy. */
	gettimeofday( &t, NULL );

	if ( g_accum_index == GST_TA_MAX_ACCUM )
		return -1;

	if ( !name )
		return -1;

	if( strlen ( name ) == 0 )
		return -1;

	/* varify the 'name' is already exist. */
	if ( (index = __get_accum_index(name)) == -1 )
	{
		printf("[%s] is not added before!\n", name);
		return -1;
	}

	accum = g_accums[index];

	/* verify pairs of begin, end. */
	if (!accum->on_estimate)
	{
		printf("[%s] is not 'begin' yet!\n", accum->name);
		accum->num_unpair ++;
		return -1;
	}
	
	/* get current timestamp. */
	tval = t.tv_sec*1000000L + t.tv_usec;
#ifdef GST_EXT_TA_UNIT_MSEC	
	tval = (tval>=1000) ? tval/1000 : 0;
#endif

	/* update last_end */
	accum->last_end = tval;

	/* make get elapsed time. */
	tval = tval - accum->timestamp;

	/* update min/max */
	accum->elapsed_max = tval > accum->elapsed_max ? tval : accum->elapsed_max;
	accum->elapsed_min = tval < accum->elapsed_min ? tval : accum->elapsed_min;

	if ( show )
		printf("[ACCUM END] %s : %ld + %u ---(%s:%d)\n", name, accum->elapsed_accum, tval, filename, line );

	/* add elapsed time */
	accum->elapsed_accum += tval;
	accum->on_estimate = 0;

	return 0;
}

void __print_some_info(FILE* fp)
{
	if (!fp)
		return;

	/* comment */
	{
		fprintf(fp, 	
"\nThe following content is automatically generated by gstreamer\n \
with GST_EXT_TIME_ANALYSIS feature. This feature can affects\n \
performance of multimedia functionality. so if you want to\n \
disable it. just comment the feature in /limo-middleware/\n \
multimedia-framework/media-service/msl_feature_config.py\n" );
	}

	/* General infomation */
	{
		time_t t_val;
		char hostname[256] = {'\0',};		
		char ctime_str[256] = {'\0',};
		struct utsname uts;
		struct rusage r_usage;

		fprintf(fp, "\n[[ General info ]]\n");
		
		/* time and date */
		time(&t_val);
		fprintf(fp, "Date : %s", ctime_r(&t_val, ctime_str) );

		/* system */
		if ( gethostname(hostname, 255) == 0 && uname(&uts) >= 0 )
		{
			fprintf(fp, "Hostname : %s\n", hostname);
			fprintf(fp, "System : %s\n", uts.sysname);
			fprintf(fp, "Machine : %s\n", uts.machine);
			fprintf(fp, "Nodename : %s\n", uts.nodename);
			fprintf(fp, "Release : %s \n", uts.release);
			fprintf(fp, "Version : %s \n", uts.version);
		}

		/* process info. */
		fprintf(fp, "Process priority : %d\n", getpriority(PRIO_PROCESS, getpid()) );
		getrusage(RUSAGE_SELF, &r_usage);
		fprintf(fp, "CPU usage : User = %ld.%06ld, System = %ld.%06ld\n", 
				r_usage.ru_utime.tv_sec, r_usage.ru_utime.tv_usec,
				r_usage.ru_stime.tv_sec, r_usage.ru_stime.tv_usec );


	}

	/* host environment variables */
	{
		extern char** environ;
		char** env = environ;

		fprintf(fp, "\n[[ Host environment variables ]]\n");
		while(*env)
		{
			fprintf(fp, "%s\n", *env);
			env++;
		}
	}
	
	/* applied target feature */
	{
		fprintf(fp, "\n[[ Applied target feature ]]\n");
		#ifdef GST_EXT_TIME_ANALYSIS
		fprintf(fp, "GST_EXT_TIME_ANALYSIS\n");
		#endif
		#ifdef GST_EXT_SKIP_RESCAN_REGISTRY
		fprintf(fp, "GST_EXT_SKIP_RESCAN_REGISTRY\n");
		#endif
		#ifdef GST_EXT_USE_TINY_REGISTRY 		

		fprintf(fp, "GST_EXT_USE_TINY_REGISTRY\n");
		#endif
		#ifdef GST_EXT_PAD_LINK_UNCHECKED
		fprintf(fp, "GST_EXT_PAD_LINK_UNCHECKED\n");
		#endif
		#ifdef GST_EXT_DFBVIDEOSINK_IPP
		fprintf(fp, "GST_EXT_DFBVIDEOSINK_IPP\n");
		#endif
		#ifdef GST_EXT_REDUCE_PLUGIN_NUM
		fprintf(fp, "GST_EXT_REDUCE_PLUGIN_NUM\n");
		#endif
		#ifdef GST_EXT_USE_PDP_NETWORK
		fprintf(fp, "GST_EXT_USE_PDP_NETWORK\n");
		#endif
		#ifdef GST_EXT_VOLUME_WITHOUT_LIBOIL
		fprintf(fp, "GST_EXT_VOLUME_WITHOUT_LIBOIL\n");
		#endif
		#ifdef GST_EXT_DECODEBIN_QUEUE_ENLARGE
		fprintf(fp, "GST_EXT_DECODEBIN_QUEUE_ENLARGE\n");
		#endif
		
		/* Camcorder releated feature */

		#ifdef GST_EXT_NONBLOCKDQUE
		fprintf(fp, "GST_EXT_NONBLOCKDQUE\n");
		#endif
		#ifdef GST_EXT_RENEGOTIATION
		fprintf(fp, "GST_EXT_RENEGOTIATION\n");
		#endif
		#ifdef GST_EXT_MOBILECAMERA
		fprintf(fp, "GST_EXT_MOBILECAMERA\n");
		#endif
		#ifdef GST_EXT_VIDEOSCALE_IPP
		fprintf(fp, "GST_EXT_VIDEOSCALE_IPP\n");
		#endif
		#ifdef GST_EXT_ASYNC_DEV
		fprintf(fp, "GST_EXT_ASYNC_DEV\n");
		#endif
		#ifdef GST_EXT_AV_RECORDING
		fprintf(fp, "GST_EXT_AV_RECORDING\n");
		#endif
		#ifdef GST_EXT_SWITCH_CAMERA
		fprintf(fp, "GST_EXT_SWITCH_CAMERA\n");
		#endif

		fprintf(fp, "\n\n");
	}

	
}

void gst_ta_accum_show_result(int direction)
{
	int i = 0;
	char format[256];
	FILE* fp = stderr;

	if (!g_accums)
		return;


	switch (direction)
	{
		case	MMTA_SHOW_STDOUT: fp = stdout;	break;
		case	MMTA_SHOW_STDERR: fp = stderr;	break;
		case	MMTA_SHOW_FILE:
		{
			fp = fopen("/tmp/gst-ta.log", "wt");
			if (!fp)
				return;
		}
		break;
	}

	__print_some_info(fp);


#ifdef GST_EXT_TA_UNIT_MSEC
	sprintf(format, "[%%3d] %%-%ds | \ttotal : %%4ld\tcalls : %%3ld\tavg : %%4ld\tmin : %%4ld\tmax : %%4ld\tstart : %%4lu\tend : %%4lu\tunpair : %%3ld\n", g_accum_longest_name);
	fprintf(fp, "BEGIN RESULT ACCUM============================ : NumOfItems : %d, unit(msec)\n", g_accum_index);
#else
	snprintf(format, sizeof(format)-1, "[%%3d] %%-%ds | \ttotal : %%ld\tcalls : %%ld\tavg : %%ld\tmin : %%ld\tmax : %%ld\tstart : %%lu\tend : %%lu\tunpair : %%ld\n", g_accum_longest_name);
	fprintf(fp, "BEGIN RESULT ACCUM============================ : NumOfItems : %d, unit(usec)\n", g_accum_index);
#endif

	for ( i = 0; i < g_accum_index; i++ )
	{	
		/* prevent 'devide by zero' error */
		if (g_accums[i]->num_calls == 0)
			g_accums[i]->num_calls = 1;
			
		fprintf(fp, 
			format, 	
			i, 
			g_accums[i]->name, 
			g_accums[i]->elapsed_accum,
			g_accums[i]->num_calls,
			(g_accums[i]->elapsed_accum == 0)?0:(int)(g_accums[i]->elapsed_accum / g_accums[i]->num_calls),
			g_accums[i]->elapsed_min,
			g_accums[i]->elapsed_max,
			g_accums[i]->first_start - g_accum_first_time,
			g_accums[i]->last_end - g_accum_first_time,
			g_accums[i]->num_unpair );
	}
	fprintf(fp, "END RESULT ACCUM  ============================\n");

	if ( direction == MMTA_SHOW_FILE )
		fclose(fp);
}

#endif	/*_MM_TA_C_*/
