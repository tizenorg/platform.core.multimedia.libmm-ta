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

#ifndef _MM_TA_H_
#define _MM_TA_H_
 
#define GST_TA_MAX_CHECKPOINT	500
#define GST_TA_MAX_ACCUM		500

typedef struct _gst_ta_checkpoint
{
	unsigned long timestamp;
	char* name;
} gst_ta_checkpoint;

typedef struct _gst_ta_accum_item
{
	unsigned long elapsed_accum;
	unsigned long num_calls;
	unsigned long elapsed_min;
	unsigned long elapsed_max;
	unsigned long first_start;
	unsigned long last_end;

	char* name;

	unsigned long timestamp;
	int on_estimate;
	int num_unpair;
} gst_ta_accum_item;

#define MMTA_SHOW_STDOUT	0
#define MMTA_SHOW_STDERR	1
#define MMTA_SHOW_FILE	2	

/* COMMON */
int gst_ta_init(void);
int gst_ta_release(void);
void gst_ta_set_enable(int enable);
char* gst_ta_fmt(const char* fmt, ...);

/* CHECK POINT */
int gst_ta_add_checkpoint(char* name, int show, char* filename, int line);
void gst_ta_show_checkpoints(void);
void gst_ta_show_diff(char* name1, char* name2);

int gst_ta_get_numof_checkpoints();
unsigned long gst_ta_get_diff(char* name1, char* name2);
char* gst_ta_get_name(int idx);

/* ACCUM ITEM */
int gst_ta_accum_item_begin(char* name, int show, char* filename, int line);
int gst_ta_accum_item_end(char* name, int show, char* filename, int line);
void gst_ta_accum_show_result(int direction);

/* macro. */
#ifdef GST_EXT_TIME_ANALYSIS
#define MMTA_INIT()								(	gst_ta_init()												)
#define MMTA_RELEASE()							(	gst_ta_release()											)
#define MMTA_SET_ENABLE(enable)				(	gst_ta_set_enable(enable)								)

/* checkpoint handling */
#define MMTA_ADD_CHECKPOINT(name,show)		(	gst_ta_add_checkpoint(name,show,__FILE__,__LINE__)		)
#define MMTA_SHOW_CHECKPOINTS()				(	gst_ta_show_checkpoints()								)
#define MMTA_SHOW_DIFF(name1, name2)			(	gst_ta_show_diff(name1, name2)							)
#define MMTA_GET_NUMOF_CHECKPOINTS()			(	gst_ta_get_numof_checkpoints()							)
#define MMTA_GET_DIFF(name1, name2)			(	gst_ta_get_diff(name1, name2)							)
#define MMTA_GET_NAME(idx)						(	gst_ta_get_name(idx)									)

/* accum item handling */
#define MMTA_ACUM_ITEM_BEGIN(name,show)		(	gst_ta_accum_item_begin(name,show,__FILE__,__LINE__)	)
#define MMTA_ACUM_ITEM_END(name,show)		(	gst_ta_accum_item_end(name,show,__FILE__,__LINE__)		)
#define MMTA_ACUM_ITEM_SHOW_RESULT()		(	gst_ta_accum_show_result(MMTA_SHOW_STDOUT)			) 
#define MMTA_ACUM_ITEM_SHOW_RESULT_TO(x)	(	gst_ta_accum_show_result(x)							) 

#define __ta__(name, x) \
MMTA_ACUM_ITEM_BEGIN(name, 0); \
x \
MMTA_ACUM_ITEM_END(name, 0);
					
#define __tafmt__(fmt, args...) 			(	gst_ta_fmt(fmt, ##args)	)

#else

#define MMTA_INIT()
#define MMTA_RELEASE()
#define MMTA_SET_ENABLE(enable)

/* checkpoint handling */
#define MMTA_ADD_CHECKPOINT(name,show)
#define MMTA_SHOW_CHECKPOINTS()
#define MMTA_SHOW_DIFF(name1, name2)
#define MMTA_GET_NUMOF_CHECKPOINTS()
#define MMTA_GET_DIFF(name1, name2)
#define MMTA_GET_NAME(idx)

/* accum item handling */
#define MMTA_ACUM_ITEM_BEGIN(name,show)
#define MMTA_ACUM_ITEM_END(name,show)
#define MMTA_ACUM_ITEM_SHOW_RESULT()
#define MMTA_ACUM_ITEM_SHOW_RESULT_TO(x)
#define __ta__(name, x) x
#define __tafmt__(fmt, args...)

#endif

#endif	/*_MM_TA_H_*/

