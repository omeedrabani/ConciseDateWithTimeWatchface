#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer;	
static TextLayer *s_border_layer;
static TextLayer *s_date_layer;

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
	if (units_changed == DAY_UNIT)
		update_date();
}

static void main_window_load(Window *window) {
	// Create time TextLayer
	s_time_layer = text_layer_create(GRect(0, 100, 144, 68));
  text_layer_set_background_color(s_time_layer, GColorBlack);
	text_layer_set_text_color(s_time_layer, GColorClear);
	
	// Improve the layout to be more like a watchface
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
	
	// Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
	// Create border TextLayer
	s_border_layer = text_layer_create(GRect(0, 0, 144, 70));
	text_layer_set_background_color(s_border_layer, GColorBlack);
	text_layer_set_text_color(s_border_layer, GColorClear);
	text_layer_set_text(s_border_layer, "");	
	
	text_layer_set_font(s_border_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(s_border_layer, GTextAlignmentCenter);
	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_border_layer));

	// Create date TextLayer
	s_date_layer = text_layer_create(GRect(0, 70, 144, 28));
	text_layer_set_background_color(s_date_layer, GColorBlack);
	text_layer_set_text_color(s_date_layer, GColorClear);
	
	text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	
	// Make sure the time and date is displayed from the start
	update_time();
	update_date();
}

static void main_window_unload(Window *window) {
	// Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_border_layer);
	text_layer_destroy(s_date_layer);
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