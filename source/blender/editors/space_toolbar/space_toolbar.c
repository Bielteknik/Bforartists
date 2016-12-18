/*
* ***** BEGIN GPL LICENSE BLOCK *****
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
* The Original Code is Copyright (C) 2008 Blender Foundation.
* All rights reserved.
*
*
* Contributor(s): Michael Neilly
*
* ***** END GPL LICENSE BLOCK *****
*/

/** \file blender/editors/space_toolbar/space_toolbar.c
*  \ingroup sptoolbar
*/

// BFA - barebone

#include <string.h>

#include "DNA_text_types.h"

#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"

#include "BKE_context.h"
#include "BKE_screen.h"

#include "ED_space_api.h"
#include "ED_screen.h"

#include "BIF_gl.h"

#include "WM_api.h"
#include "WM_types.h"

#include "UI_interface.h"
#include "UI_resources.h"
#include "UI_view2d.h"

static SpaceLink *toolbar_new(const bContext *C)
{
	ScrArea *sa = CTX_wm_area(C);
	ARegion *ar;
	SpaceToolbar *stoolbar;

	stoolbar = MEM_callocN(sizeof(SpaceToolbar), "inittoolbar");
	stoolbar->spacetype = SPACE_TOOLBAR;

	/* header */
	ar = MEM_callocN(sizeof(ARegion), "header for toolbar");

	BLI_addtail(&stoolbar->regionbase, ar);
	ar->regiontype = RGN_TYPE_HEADER;
	ar->alignment = RGN_ALIGN_BOTTOM;

	/* main area */
	ar = MEM_callocN(sizeof(ARegion), "main area for toolbar");

	BLI_addtail(&stoolbar->regionbase, ar);
	ar->regiontype = RGN_TYPE_WINDOW;

	return (SpaceLink *)stoolbar;
}

/* add handlers, stuff you only do once or on area/region changes */
static void toolbar_main_area_init(wmWindowManager *wm, ARegion *ar)
{
	UI_view2d_region_reinit(&ar->v2d, V2D_COMMONVIEW_CUSTOM, ar->winx, ar->winy);
}

static void toolbar_main_area_draw(const bContext *C, ARegion *ar)
{
	/* draw entirely, view changes should be handled here */
	SpaceToolbar *stoolbar = CTX_wm_space_toolbar(C);
	View2D *v2d = &ar->v2d;
	View2DScrollers *scrollers;

	/* clear and setup matrix */
	UI_ThemeClearColor(TH_BACK);
	glClear(GL_COLOR_BUFFER_BIT);

	/* works best with no view2d matrix set */
	UI_view2d_view_ortho(v2d);

	/* reset view matrix */
	UI_view2d_view_restore(C);

	/* scrollers */
	scrollers = UI_view2d_scrollers_calc(C, v2d, V2D_ARG_DUMMY, V2D_ARG_DUMMY, V2D_ARG_DUMMY, V2D_GRID_CLAMP);
	UI_view2d_scrollers_draw(C, v2d, scrollers);
	UI_view2d_scrollers_free(scrollers);
}

static void toolbar_header_area_init(wmWindowManager *UNUSED(wm), ARegion *ar)
{
	ED_region_header_init(ar);
}

static void toolbar_header_area_draw(const bContext *C, ARegion *ar)
{
	ED_region_header(C, ar);
}

/********************* registration ********************/

/* only called once, from space/spacetypes.c */
void ED_spacetype_toolbar(void)
{
	SpaceType *st = MEM_callocN(sizeof(SpaceType), "spacetype toolbar");
	ARegionType *art;

	st->spaceid = SPACE_TOOLBAR;
	strncpy(st->name, "Toolbar", BKE_ST_MAXNAME);

	st->new = toolbar_new;

	/* regions: main window */
	art = MEM_callocN(sizeof(ARegionType), "spacetype toolbar region");
	art->regionid = RGN_TYPE_WINDOW;

	art->init = toolbar_main_area_init;
	art->draw = toolbar_main_area_draw;

	BLI_addhead(&st->regiontypes, art);

	/* regions: header */
	art = MEM_callocN(sizeof(ARegionType), "spacetype toolbar region");
	art->regionid = RGN_TYPE_HEADER;
	art->prefsizey = HEADERY;
	art->keymapflag = ED_KEYMAP_UI | ED_KEYMAP_VIEW2D | ED_KEYMAP_HEADER;
	art->init = toolbar_header_area_init;
	art->draw = toolbar_header_area_draw;

	BLI_addhead(&st->regiontypes, art);

	BKE_spacetype_register(st);
}