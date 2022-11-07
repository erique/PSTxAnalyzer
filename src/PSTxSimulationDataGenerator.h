#ifndef PSTX_SIMULATION_DATA_GENERATOR
#define PSTX_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>
#include <SimulationChannelDescriptor.h>

class PSTxAnalyzerSettings;

class PSTxSimulationDataGenerator
{
  public:
    PSTxSimulationDataGenerator();
    ~PSTxSimulationDataGenerator();

    void Initialize( U32 simulation_sample_rate, PSTxAnalyzerSettings* settings );
    U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

  protected:
    PSTxAnalyzerSettings* mSettings;
    U32 mSimulationSampleRateHz;

  protected:
    ClockGenerator mClockGenerator;
    U64 mValue;

    void CreatePSTxData();

    SimulationChannelDescriptorGroup mPSTxSimulationChannels;
    SimulationChannelDescriptor* mPST0;
    SimulationChannelDescriptor* mPST1;
    SimulationChannelDescriptor* mPST2;
    SimulationChannelDescriptor* mPST3;
    SimulationChannelDescriptor* mPST4;
    SimulationChannelDescriptor* mCLK;
    SimulationChannelDescriptor* mTRIG;
};

#endif //PSTX_SIMULATION_DATA_GENERATOR
