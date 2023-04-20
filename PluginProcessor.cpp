/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
KeblexCompAudioProcessor::KeblexCompAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

KeblexCompAudioProcessor::~KeblexCompAudioProcessor()
{
}

//==============================================================================
const juce::String KeblexCompAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KeblexCompAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool KeblexCompAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool KeblexCompAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double KeblexCompAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KeblexCompAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int KeblexCompAudioProcessor::getCurrentProgram()
{
    return 0;
}

void KeblexCompAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String KeblexCompAudioProcessor::getProgramName (int index)
{
    return {};
}

void KeblexCompAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void KeblexCompAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    timeElapsed = 0.0;
    curCompState = OFF;
}

void KeblexCompAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KeblexCompAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void KeblexCompAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    static int counter = 0; //to track number of samples overtime

    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
   
    //Aplica input gain ao buffer inteiro de uma só vez
    float linInGain = powf(10, curInGain / 20.0);
    buffer.applyGain(linInGain);

    float chMagnitude; //Calcula a magnitude para todos os canais (seja através de PEAK ou RMS)

    //percorre cada canal para o  current frame
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        //Para a deteção PEAK
        if (detMode == PEAK)
        {
            chMagnitude = buffer.getMagnitude(ch, 0, buffer.getNumSamples());
        }
        else if (detMode == RMS)
        {
            chMagnitude = buffer.getRMSLevel(ch, 0, buffer.getNumSamples());
        }

        //Percorre cada frame para esta callback
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            //Obter o sinal e o valor absoluto da sample atual
            float curSample = buffer.getSample(ch, n);
            float sign = (curSample > 0) - (curSample < 0);
            float curSampleAbs = fabsf(curSample);

            //Converter valores em dB para linear amplitude
            float linThreshold = powf(10, curThresh / 20.0);
            float linOutGain = powf(10, curOutGain / 20.0);

            //Cada amostra tem o que chamarei de 'base' (a parte da sample até o valor do threshold) e um remaind (a parte da sample, se houver, que excede o limite)
            float base = (curSampleAbs > linThreshold) ? linThreshold : curSampleAbs;
            float remainder = (curSampleAbs > linThreshold) ? (curSampleAbs - linThreshold) : 0.0;

            //Ajustar o ratio - converter de dB para magnitude
            float linRatio = 1.0 / powf(10, ((float)curRatio) / 20.0);

            // Calcular a nova sample, comprimida ou não
            float compressedSampleVal = (base + (remainder * linRatio)) * sign;
            float interpVal = getInterpCompVal(chMagnitude, curSample, compressedSampleVal, linThreshold);

            buffer.setSample(ch, n, interpVal * linOutGain);
        }

        //Calcular o RMS e exibi-lo
        float curMag = buffer.getMagnitude(ch, 0, buffer.getNumSamples());
        curSampleVal = curMag; //O valor de potência exibido na interface gráfica (GUI)
    }
}

//==============================================================================
bool KeblexCompAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* KeblexCompAudioProcessor::createEditor()
{
    return new KeblexCompAudioProcessorEditor (*this);
}

//==============================================================================
void KeblexCompAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void KeblexCompAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KeblexCompAudioProcessor();
}

float KeblexCompAudioProcessor::getInterpCompVal(float valToCheck, float unCompressedVal, float compressedVal, float linearThreshold)
{
    switch (curCompState)
    {
    case(ATTACK):
        if (timeElapsed < curAtkTime) 
        {
            //Atualizar quanto tempo passou no estado atual
            timeElapsed += 1 / getSampleRate();
            //Retornar o valor interpolado linearmente comprimido se o não comprimido estiver acima do threshold
            if (unCompressedVal > linearThreshold)
            {
                return ((timeElapsed * compressedVal) +
                    ((curAtkTime - timeElapsed) * unCompressedVal)) * 0.5 / curAtkTime;
            }
            else
            {
                return unCompressedVal;
            }

        }
        else //Caso contrário, mudar para o próximo estado
        {
            changeCompressorState(curCompState, ACTIVE);
            return compressedVal;
        }
        break;

    case(ACTIVE):
        if (valToCheck <= linearThreshold) //Mudar o estado para 'release' se o peak geral tiver caído abaixo do valor do threshold
        {
            changeCompressorState(curCompState, RELEASE);
        }

        if (unCompressedVal > linearThreshold) //Ainda assim, apenas comprimimos os valores individuais que estão acima do threshold
        {
            return compressedVal;
        }
        else
        {
            return unCompressedVal;
        }

        break;

    case(RELEASE):
        if (timeElapsed < curRelTime) //Se ainda deveríamos estar no estado 'release'
        {
            timeElapsed += 1 / getSampleRate();
            //Retornar o valor interpolado linearmente

            if (unCompressedVal > linearThreshold)
            {
                return (((curRelTime - timeElapsed) * compressedVal) +
                    (timeElapsed * unCompressedVal)) *
                    0.5 / curRelTime;
            }
            else
            {
                return unCompressedVal;
            }

        } //Caso contrário, mudar para o próximo estado
        else
        {
            changeCompressorState(curCompState, OFF);
            return unCompressedVal;
        }
        break;

    case(OFF):
        if (valToCheck > linearThreshold)
        {
            changeCompressorState(curCompState, ACTIVE);
        }

        return unCompressedVal;

        break;

    default:
        return 0.0;
        break;
    }
}
void KeblexCompAudioProcessor::changeCompressorState(CompressorState prevCompState, CompressorState newCompState)
{
    this->curCompState = newCompState;
    this->timeElapsed = 0.0;
}
