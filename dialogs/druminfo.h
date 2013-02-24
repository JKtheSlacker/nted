#ifndef DRUM_INFO_H

#define DRUM_INFO_H

extern struct d_info_str {
	const char *name, *lily_short_name;
	int pitch;
	int note_head;
	const char *modifier;
	int line;
} default_dinfo[], current_dinfo[];

int getNumberOfDrumTabElements();

void make_default_drums_current();


#endif /* DRUM_INFO_H */
