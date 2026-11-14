#pragma once

class Health
{
public:
    explicit Health(int maximum);

    int current() const;
    int maximum() const;
    float fraction() const;
    bool isDead() const;

    void damage(int amount);
    void heal(int amount);

private:
    int m_current;
    int m_maximum;
};