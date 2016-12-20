/*
 * imessage2txt
 *
 * Written by Paul L Daniels (Inflex), 21 December 2016
 *
 * <insert BSD Revised licence>
 * <insert MIT licence>
 *
 * Query option #1
 * SELECT datetime(message.date, 'unixepoch', '+31 years', '+10 hours'), message.is_from_me, handle.id, message.text FROM message, handle WHERE message.handle_id = handle.ROWID order by message.date;
 * 
 * Query option #2
 *  SELECT datetime(message.date, 'unixepoch', '+31 years', '+10 hours'), handle.id, case message.is_from_me when '0' then "<=-" when '1' then "-=>" end as messagedir, message.text FROM message, handle WHERE message.handle_id = handle.ROWID group by handle.ROWID order by message.date;
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

char *g_input_file;

static int callback( void *NotUsed, int argc, char **argv, char **azColName ) {

	char *n="", *datecode, *handle_id, *is_from_me, *message_text;

	datecode = argv[0]?argv[0]:n;
	handle_id = argv[1]?argv[1]:n;
	is_from_me = argv[2]?argv[2]:n;
	message_text = argv[3]?argv[3]:n;

	/* deliberate obfusication of output */
	/*
	if (strlen(handle_id) > 7) handle_id[6] = '\0';

	{
		char *p = message_text;

		if (strlen(message_text) > 20) message_text[19] = '\0';
		while (*p) { 
			*p = '.';
			p++; if (*p == '\0') break;
			p++; if (*p == '\0') break;
			p++; if (*p == '\0') break;
		}
	}
	*/
	/* end of debugging / custom  output */


	fprintf(stdout,"%s, %s, %s, %s\n"
			, datecode
			, handle_id
			, (is_from_me[0]=='1')?"SENT":"RECV"
			, message_text
		   );

	return 0;

}

int parse_parameters( int argc, char **argv ) {
	int i;

	for (i=0; i < argc; i++) {

		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
				case 'i':
					/* set the input file*/
					i++;
					if (i < argc) {
						g_input_file = argv[i];
					} else {
						fprintf(stderr,"Insuffient parameters\n");
						exit(1);
					}
					break;

				default:
					break;
			} // switch
		} // if
	} // for

	return 0;
}


int main( int argc, char **argv ) {
	sqlite3 *db;
	char sql[1024];
	char *zErrMsg = 0;
	int rc;

	parse_parameters( argc, argv );

	/*
	 * If the input file is non-null, try open...
	 */
	if (g_input_file) {

		rc = sqlite3_open( g_input_file, &db );

		if ( rc ) {
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return (1);
		}

		snprintf(sql, sizeof(sql), "SELECT datetime(message.date, 'unixepoch', '+31 years', '+10 hours'), handle.id, message.is_from_me, message.text FROM message, handle WHERE message.handle_id = handle.ROWID order by message.date;");
		rc = sqlite3_exec( db, sql, callback, 0, &zErrMsg);
		if ( rc != SQLITE_OK ) {
			fprintf(stderr,"SQL Error: %s\n", zErrMsg);
		}

		sqlite3_close(db);

	}

	return 0;

}
