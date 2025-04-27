#ifndef BUTTON_INTERRUPTS_H
#define BUTTON_INTERRUPTS_H

#include <vector>
#include <functional>

class Button
{
public:
    using Callback = std::function<void()>;
    Button(int pin);

    void begin();

    void addObserver(const Callback &callback)
    {
        observers.push_back(callback);
    }

    void notify();
    void toggle() { _ioToggle ^= 0x1; }
    uint32_t getIOToggle() const { return _ioToggle; }
    void incrementCount() { _isrCount++; }
    uint32_t getISRCount() { return _isrCount; }
    uint32_t getPin() const { return _pin; }
    gpio_num_t getGPIO() const { return _gpioNum; }

private:
    std::vector<Callback> observers;
    uint32_t _pin;
    uint32_t _ioToggle = 0x1;
    uint32_t _isrCount = 0;
    gpio_num_t _gpioNum;
    // Debounce delay
    const unsigned long DEBOUNCE_DELAY = 200;
    unsigned long lastDebounceTime = 0;
};

class ButtonManager
{
public:
    template <typename... Pins>
    ButtonManager(Pins... pins)
    {
        addButtons(pins...);
    }

    void begin();

    void registerObserver(int pin, const Button::Callback &callback)
    {
        for (auto &button : buttons)
        {
            if (button->getPin() == pin)
            {
                button->addObserver(callback);
                break;
            }
        }
    }

private:
    template <typename... Pins>
    void addButtons(Pins... pins)
    {
        (addButton(new Button(pins)), ...);
    }

    void addButton(Button *button)
    {
        buttons.push_back(button);
    }

private:
    std::vector<Button *> buttons;
};

void buttonBegin();
void IRAM_ATTR handleButton1();
void IRAM_ATTR handleButton2();
void IRAM_ATTR handleButton3();

#endif