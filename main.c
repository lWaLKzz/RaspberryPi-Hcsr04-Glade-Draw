#include <stdio.h>
#include <wiringPi.h> 
#include <gtk/gtk.h> 
#include <math.h> 
#include <cairo.h> 

#define ECHO 28
#define TRIG 29


float distanceHCSR;

GtkBuilder      *builder; 
GtkWidget       *window;
GtkLabel		*label;
GtkWidget       *area;

void setup(void);
gboolean time_handler(GtkLabel *label);
float Read(void);

PI_THREAD (hcsr) 
{
	while(1){
		distanceHCSR = Read();
		delay(500);
		printf("%.1fcm\n",distanceHCSR);
	}
}

gboolean on_area_draw(GtkDrawingArea *widget,cairo_t *cr){
	GtkWidget *win = gtk_widget_get_toplevel(window);
  
	int width, height;
	gtk_window_get_size(GTK_WINDOW(win), &width, &height); 

	cairo_set_line_width(cr, 9); 
	cairo_set_source_rgb(cr, 0.69, 0.19, 0); 

	cairo_translate(cr, width/2, height/4); 
	cairo_arc(cr, 0, 0, distanceHCSR*2, 0, 2 * M_PI);
	
	cairo_fill(cr); 
	return FALSE;
}


int main(int argc, char *argv[])
{	
	setup();
    
	
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window.glade", NULL); 
    window = GTK_WIDGET(gtk_builder_get_object(builder, "win")); 
    label = GTK_LABEL(gtk_builder_get_object(builder, "read_label")); 
    area = GTK_WIDGET(gtk_builder_get_object(builder, "area")); 
   
  
    
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_widget_show(window);
    g_timeout_add(500, (GSourceFunc)time_handler,label);
         
    gtk_main();
    return 0;
}

void setup(void){ 
	wiringPiSetup(); 
	piThreadCreate(hcsr); 
	pinMode(ECHO,INPUT); 
	pinMode(TRIG,OUTPUT);
	digitalWrite(TRIG,LOW);
	delay(100);
}

gboolean time_handler(GtkLabel *label){
	
	char convert[50];
	sprintf(convert,"Distance : %.1f cm",distanceHCSR);
	gtk_label_set_text(label,convert); 
	gtk_widget_queue_draw(area); 
	
    return TRUE;
}

float Read(void){
	digitalWrite(TRIG,HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG,LOW);
	long startTime;
	long stopTime;
	while(digitalRead(ECHO)==0){startTime = micros();}
	while(digitalRead(ECHO)==1){stopTime = micros();}
	long travelTime = stopTime - startTime;
	float distance = (travelTime * 34300) / 2;
	return distance/1000000;
}

