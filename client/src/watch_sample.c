#include <tizen.h>
#include <http.h>
#include <stdio.h>
#include <stdlib.h>
#include "watch_sample.h"

#define TEXT_BUF_SIZE 256

/* Default UI and watch data */
typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	http_session_h session; // HTTP session for POST

	int code; // TEMP for successful init - REMOVE

} appdata_s;

/* Store header information */
typedef struct header {
	char* token; // CSRF token

} header_s;

header_s* init_header()
{
	/* Initialize and return a header struct */

	header_s* header = malloc(sizeof(header_s));
	header->token = malloc(sizeof(char) * 100); // Switch to static if max size is known

	return header;
}

void deinit_header(header_s* header)
{
	/* Deinitialize a header struct */

	free(header->token);
	free(header);
}

/* Store SMS Message data */
typedef struct message {
	char to_address[20];
	char from_address[20];
	char message_body[TEXT_BUF_SIZE];

} message_s;

void got_header_callback(http_transaction_h transac, char* full_header, size_t length, void* header_struct)
{
	/* Get and store HTTP header data */
	// IMPORTANT - GET and return CSRF TOKEN

	// TEST BELOW - REMOVE ALL
	header_s* header = (header_s*) header_struct; // Static cast to header data structure
	strcpy(header->token, "TOKEN SET SUCCESSFULLY"); // Copy over placeholder
}

void transaction_completed_callback(http_transaction_h transac, char* body, void* data)
{
	/* Callback to record completed HTTP transactions */
	//TODO
}

void transaction_failed_callback(http_transaction_h transac, int reason, void* data)
{
	/* Callback to record failed HTTP transactions */
	//TODO
}

static void send_POST(appdata_s *ad)
{
	/* Send message data from client (watch) to server */

	int code;
	http_transaction_h transact = NULL;
	header_s* header = init_header(); // Create header data structure

	/* Prepare for HTTP transactions */
	code = http_session_open_transaction(ad->session, HTTP_METHOD_POST, &transact);
	if(code != HTTP_ERROR_NONE)
		dlog_print(DLOG_DEBUG, "POST", "HTTP transaction opened");
	else
	{
		dlog_print(DLOG_DEBUG, "POST", "Failed to open transaction");
		return;
	}

	/* Set callback for when header information is received */
	http_transaction_set_received_header_cb(transact, got_header_callback, header);
	/* Set callback for when a transaction is completed */
	http_transaction_set_completed_cb(transact, transaction_completed_callback, NULL);
	/* Set callback for when a transaction fails */
	http_transaction_set_aborted_cb(transact, transaction_failed_callback, NULL);
}

static void initialize_HTTP(appdata_s *ad)
{
	/* Set up HTTP for client */

	bool auto_redirect = true;

	/* Initialize HTTP functionality */
	int code = http_init();
	if (code != HTTP_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "HTTP Initialization", "HTTP initialization failed.");
		ad->code = -1; // TEMP
		return;
	}

	/* Initialize  HTTP session */
	code = http_session_create(HTTP_SESSION_MODE_NORMAL, &(ad->session));
	if (code != HTTP_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "HTTP Session", "HTTP session failed.");
		ad->code = -1; // TEMP
		return;
	}

	dlog_print(DLOG_DEBUG, "HTTP", "HTTP is good to go!");
	ad->code = 1; // TEMP

	/* Automatically redirect connections */
	code = http_session_set_auto_redirection(ad->session, auto_redirect);
	if (code != HTTP_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, "HTTP Redirect", "HTTP Redirect failed.");
		ad->code = -1; // TEMP - Debug
		return;
	}
}

static void deinitialize_HTTP(appdata_s *ad)
{
	/* Clean up HTTP functionality */

	/* Deinitialize HTTP */
	int code = http_deinit();

	if (code == HTTP_ERROR_NONE) // DEBUG
		dlog_print(DLOG_DEBUG, "HTTP Deinitialization", "HTTP deinitialized.");
	else
		dlog_print(DLOG_DEBUG, "HTTP Deinitialization", "HTTP deinitialization failed.");

	// TODO - Destroy all transactions
	// TODO - Destroy sessions

}

static void
update_watch(appdata_s *ad)
{
	char watch_text[TEXT_BUF_SIZE];

	if(ad->code > 0) // Placeholder watch text
		snprintf(watch_text, TEXT_BUF_SIZE, "<align=center>HTTP Session Active<br>-<br>Transaction Opened</align>");
	else
		snprintf(watch_text, TEXT_BUF_SIZE, "<align=center>HTTP Session Failed</align>");

	elm_object_text_set(ad->label, watch_text);
}

static void
create_base_gui(appdata_s *ad, int width, int height)
{
	int ret;
	watch_time_h watch_time = NULL;

	/* Window */
	ret = watch_app_get_elm_win(&ad->win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ret);
		return;
	}

	evas_object_resize(ad->win, width, height);

	/* Conformant */
	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Label*/
	ad->label = elm_label_add(ad->conform);
	evas_object_resize(ad->label, width, height / 3);
	evas_object_move(ad->label, 0, height / 3);
	evas_object_show(ad->label);

	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d", ret);

	update_watch(ad);
	watch_time_delete(watch_time);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(int width, int height, void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */

	appdata_s *ad = data;

	/* Configure client for HTTP POST */
	initialize_HTTP(data);

	// TODO - get_Message()

	/* Send message from client to server */
	send_POST(data); //Send POST

	create_base_gui(ad, width, height);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
	deinitialize_HTTP(data);
}

static void
app_time_tick(watch_time_h watch_time, void *data)
{
	/* Called at each second while your app is visible. Update watch UI. */
	appdata_s *ad = data;
	update_watch(ad);
}

static void
app_ambient_tick(watch_time_h watch_time, void *data)
{
	/* Called at each minute while the device is in ambient mode. Update watch UI. */
	appdata_s *ad = data;
	update_watch(ad);
}

static void
app_ambient_changed(bool ambient_mode, void *data)
{
	/* Update your watch UI to conform to the ambient mode */
}

static void
watch_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	app_event_get_language(event_info, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
watch_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	watch_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = app_ambient_tick;
	event_callback.ambient_changed = app_ambient_changed;

	watch_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
		APP_EVENT_LANGUAGE_CHANGED, watch_app_lang_changed, &ad);
	watch_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
		APP_EVENT_REGION_FORMAT_CHANGED, watch_app_region_changed, &ad);

	ret = watch_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d", ret);
	}

	return ret;
}
