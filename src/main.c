#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer;	
static TextLayer *s_border_layer;
static TextLayer *s_date_layer;
static TextLayer *s_weather_layer;


static AppSync sync;
static uint8_t sync_buffer[64];


enum {
  TEMPERATURE = 0x0
};

void sync_tuple_changed_callback(const uint32_t key,
        const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
   
  switch (key) {
    case TEMPERATURE:
      text_layer_set_text(s_weather_layer, new_tuple->value->cstring);
      break;
  }
}

static void request_weather(void) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
    // Error creating outbound message
    return;
  }

  int value = 1;
  dict_write_int(iter, 1, &value, sizeof(int), true);
  dict_write_end(iter);

  app_message_outbox_send();
}

// http://stackoverflow.com/questions/21150193/logging-enums-on-the-pebble-watch/21172222#21172222
char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}




void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "... Sync Error: %s", translate_error(app_message_error));
}


static void update_time() {
	// Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
	
	// Create a long-lived buffer
  static char buffer[] = "00:00";

	// Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%l:%M", tick_time);
  }
	
	// Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
	
}

static void update_date(){
	// Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
	
	// Create a long-lived buffer
  static char buffer[] = "..............";
	
	strftime(buffer, sizeof(buffer), "%a %b %e", tick_time);
	
	// Display this time on the TextLayer
  text_layer_set_text(s_date_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	update_date();
}

static void init_clock(Window *window) {
	// Create time TextLayer
	s_time_layer = text_layer_create(GRect(0, 100, 144, 68));
  text_layer_set_background_color(s_time_layer, GColorBlack);
	text_layer_set_text_color(s_time_layer, GColorClear);
	
	// Improve the layout to be more like a watchface
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
	
	// Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void init_date(Window *window) {
	// Create date TextLayer
	s_date_layer = text_layer_create(GRect(0, 70, 144, 28));
	text_layer_set_background_color(s_date_layer, GColorBlack);
	text_layer_set_text_color(s_date_layer, GColorClear);
	
	text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

static void init_location_search(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  //GRect bounds = layer_get_bounds(window_layer);
 
  s_weather_layer = text_layer_create(GRect(0,0,144,70));
  text_layer_set_text(s_weather_layer, "Loading...");
  text_layer_set_text_color(s_weather_layer, GColorClear);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_weather_layer, GColorBlack);
  text_layer_set_overflow_mode(s_weather_layer, GTextOverflowModeFill);
  text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(s_weather_layer));
 
  Tuplet initial_values[] = {
     TupletCString(TEMPERATURE, "Loading...")
  };
 
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);
}

static void main_window_load(Window *window) {
	
	
	// Create border TextLayer
	/*
	s_border_layer = text_layer_create(GRect(72, 0, 72, 70));
	text_layer_set_background_color(s_border_layer, GColorBlack);
	text_layer_set_text_color(s_border_layer, GColorClear);
	text_layer_set_text(s_border_layer, "");	
	
	text_layer_set_font(s_border_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(s_border_layer, GTextAlignmentCenter);
	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_border_layer));
	*/

	init_location_search(window);
	init_clock(window);
	init_date(window);
	
	// Make sure the time and date is displayed from the start
	update_time();
	update_date();
	request_weather();
}

static void main_window_unload(Window *window) {
	// Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_border_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_weather_layer);
}

static void init() {
	// Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
	
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
	// To send and receive messages
	app_message_open(64, 64);
}

static void deinit() {
	// Destroy Window
  window_destroy(s_main_window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}