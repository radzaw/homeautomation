int wait_for(int expect)
{
    uint then = time_us_32();
    while (expect != gpio_get(DHT_PIN))
    {
        sleep_us(10);
        if (time_us_32() - then > 5000)
            return 0;
    }

    return  time_us_32() - then;
}

void read_from_dht(dht_reading *result) {
    int data[5] = {0, 0, 0, 0, 0};
    uint last = 1;
    uint j = 0;
    cyw43_arch_gpio_put(LED_PIN, 1);

    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(10);

    gpio_set_dir(DHT_PIN, GPIO_IN);

    sleep_us(40);
    wait_for(0);
    wait_for(1);
    wait_for(0);

    for (uint8_t bit = 0; bit < 40; ++bit)
    {
        wait_for(1);
        uint8_t count = wait_for(0);
        data[bit / 8] <<= 1;
        if (count > 50)
        {
            data[bit / 8] |= 1;
        }
    }

    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 1);

    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
    {
        float _h = (float) ((data[0] << 8) + data[1]) / 10;
        float _t = (float) (((data[2] & 0x7F) << 8) + data[3]) / 10;

        if (data[2] & 0x80)
        {
            _t = -_t;
        }

        if (isnan(_t) || isnan(_h) || _t == 0)
        {
            result->temp_celsius = 0;
            result->humidity = 0;
            printf("bad data");
        }
        else
        {
            result->humidity = _h;
            result->temp_celsius = _t;
        }
    }
    else
    {
        result->temp_celsius = 0;
        result->humidity = 0;
        printf("bad data");
    }

    cyw43_arch_gpio_put(LED_PIN, 0);
}
