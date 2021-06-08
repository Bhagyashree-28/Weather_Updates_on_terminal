#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <json-c/json.h>
#include "./include/myheader.h"
void parsejson(char *city, char buf[2025])
{
    //creating json file for particular city passed in commandline
    char *ext = ".json";
    char file_name[15] = "";
    strcat(file_name, city);
    strcat(file_name, ext);
    //printf("%s\n",file_name );
    FILE *fp1;
    fp1 = fopen(file_name, "w+");
    if ((fp1 = fopen(file_name, "w+")) == NULL)
    {
        printf("Error in file\n");
    }
    //fetching only json data from api response and storing in particular json file
    char str = '{';
    int index;
    for (int i = 0; i < strlen(buf); i++)
    {
        if (str == buf[i])
        {
            index = i;
            break;
        }
    }
    char buffu[2024];

    int i = 0;

    for (int j = index; j < strlen(buf); j++)
    {

        buffu[i] = buf[j];
        i++;

        if (buf[j] == '\0')
            break;
    }
    fprintf(fp1, "%s", buffu);
    rewind(fp1);

    //parsing json data
    char data[2048];
    struct json_object *parsed_json; //creating json object
    struct json_object *List, *city_name, *country;
    struct json_object *main, *mintemp, *maxtemp, *feellike, *temperature, *pressure, *humidity;
    struct json_object *temp, *temp1;
    struct json_object *weather, *weather_desc;
    size_t list_size, w_size;

    fread(data, 2048, 1, fp1); //reading json file in character array variable
    fclose(fp1);
    // printf("%s\n",data );
    parsed_json = json_tokener_parse(data);
    json_object_object_get_ex(parsed_json, "list", &List);
    list_size = json_object_array_length(List);

    for (i = 0; i < list_size; i++)
    {
        temp = json_object_array_get_idx(List, i);
    }
    json_object_object_get_ex(temp, "name", &city_name);
    printf("city name: %s\n", json_object_get_string(city_name));

    json_object_object_get_ex(temp, "main", &main);
    json_object_object_get_ex(main, "temp", &temperature);
    json_object_object_get_ex(main, "feels_like", &feellike);
    json_object_object_get_ex(main, "temp_min", &mintemp);
    json_object_object_get_ex(main, "temp_max", &maxtemp);
    json_object_object_get_ex(main, "pressure", &pressure);
    json_object_object_get_ex(main, "humidity", &humidity);
    printf("Temperature: %d\n", json_object_get_int(temperature));
    printf("Feels Like: %d\n", json_object_get_int(feellike));
    printf("Min temp: %d\n", json_object_get_int(mintemp));
    printf("Max temp: %d\n", json_object_get_int(maxtemp));
    printf("Pressure: %d\n", json_object_get_int(pressure));
    printf("Humidity: %d\n", json_object_get_int(humidity));

    json_object_object_get_ex(temp, "weather", &weather);
    w_size = json_object_array_length(weather);
    for (i = 0; i < w_size; i++)
    {
        temp1 = json_object_array_get_idx(weather, i);
    }
    json_object_object_get_ex(temp1, "description", &weather_desc);
    printf("Description: %s\n\n", json_object_get_string(weather_desc));
}
