struct BlinkDef{
    bool isOn[20] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
    short durations[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool repeats = false;
    unsigned short currentIndex = 0;
    unsigned long nextUpdateTime = millis();
};
