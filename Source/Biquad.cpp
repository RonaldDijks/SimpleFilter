/*
  ==============================================================================

    Biquad.cpp
    Created: 21 May 2020 4:00:27pm
    Author:  Production

  ==============================================================================
*/

#include "Biquad.h"
#include "JuceHeader.h"

double Biquad::process(double input)
{
    // process phase

    double y = input * m_a0 +
        m_xn1 * m_a1 + 
        m_xn2 * m_a2 - 
        m_yn1 * m_b1 - 
        m_yn2 * m_b2;

    // state update phase
    m_xn2 = m_xn1;
    m_xn1 = input;

    m_yn2 = m_yn1;
    m_yn1 = y;

    return y;
}

void Biquad::setCutoffFrequency(double frequency)
{
    m_cutoffFrequency = frequency;
    calculateCoefficients();
}

void Biquad::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;
    calculateCoefficients();
}

void Biquad::setQ(double q)
{
    m_q = q;
    calculateCoefficients();
}

void Biquad::setFilterType(FilterType filterType)
{
    m_filterType = filterType;
    calculateCoefficients();
}

void Biquad::calculateCoefficients()
{
    auto thetaC = MathConstants<double>::twoPi * m_cutoffFrequency / m_sampleRate;
    auto d = 1. / m_q;
    auto beta = 0.5 * ((1. - (d / 2.) * sin(thetaC)) / (1. + (d / 2.) * sin(thetaC)));
    auto gamma = (0.5 + beta) * cos(thetaC);

    switch (m_filterType)
    {
    case FilterType::LowPass: {
        m_a1 = 0.5 + beta - gamma;
        m_a0 = m_a1 / 2.;
        m_a2 = m_a0;

        m_b1 = -2. * gamma;
        m_b2 = 2. * beta;
        break;
    }
    case FilterType::HighPass: {
        m_a0 = (0.5 * beta + gamma) / 2;
        m_a1 = -(0.5 * beta + gamma);
        m_a2 = (0.5 * beta + gamma) / 2;

        m_b1 = -2. * gamma;
        m_b2 = 2. * beta;
        break;
    }
    }
}

Biquad::Biquad()
{
    m_cutoffFrequency = 440;
    m_sampleRate = 44100;
    m_filterType = FilterType::LowPass;
    
    m_a0 = 0;
    m_a1 = 0;
    m_a2 = 0;

    m_b1 = 0;
    m_b2 = 0;

    m_xn1 = 0;
    m_xn2 = 0;
    m_yn1 = 0;
    m_yn2 = 0;

    calculateCoefficients();
}

Biquad::~Biquad()
{
}
