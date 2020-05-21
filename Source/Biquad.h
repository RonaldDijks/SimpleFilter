/*
  ==============================================================================

    Biquad.h
    Created: 21 May 2020 4:00:27pm
    Author:  Production

  ==============================================================================
*/

#pragma once



class Biquad {

public:
    enum class FilterType
    {
        LowPass,
        HighPass
    };

    Biquad();
    ~Biquad();

    double process(double input);
    void setCutoffFrequency(double frequency);
    void setSampleRate(double sampleRate);
    void setQ(double q);
    void setFilterType(FilterType filterType);

private:
    double m_a0;
    double m_a1;
    double m_a2;

    double m_b1;
    double m_b2;

    double m_xn1;
    double m_xn2;
    double m_yn1;
    double m_yn2;

    double m_cutoffFrequency;
    double m_sampleRate;
    double m_q;
    FilterType m_filterType;

    void calculateCoefficients();
};