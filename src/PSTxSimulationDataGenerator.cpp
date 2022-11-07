#include "PSTxSimulationDataGenerator.h"
#include "PSTxAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

PSTxSimulationDataGenerator::PSTxSimulationDataGenerator()
{
}

PSTxSimulationDataGenerator::~PSTxSimulationDataGenerator()
{
}

void PSTxSimulationDataGenerator::Initialize( U32 simulation_sample_rate, PSTxAnalyzerSettings* settings )
{
    mSimulationSampleRateHz = simulation_sample_rate;
    mSettings = settings;

    mClockGenerator.Init( simulation_sample_rate / 10, simulation_sample_rate );

    mPST0 = mPSTxSimulationChannels.Add( settings->mPST0Channel, mSimulationSampleRateHz, BIT_LOW );
    mPST1 = mPSTxSimulationChannels.Add( settings->mPST1Channel, mSimulationSampleRateHz, BIT_LOW );
    mPST2 = mPSTxSimulationChannels.Add( settings->mPST2Channel, mSimulationSampleRateHz, BIT_LOW );
    mPST3 = mPSTxSimulationChannels.Add( settings->mPST3Channel, mSimulationSampleRateHz, BIT_LOW );
    mPST4 = mPSTxSimulationChannels.Add( settings->mPST4Channel, mSimulationSampleRateHz, BIT_LOW );
    mCLK = mPSTxSimulationChannels.Add( settings->mCLKChannel, mSimulationSampleRateHz, BIT_LOW );

    if( settings->mTRIGChannel != UNDEFINED_CHANNEL )
        mTRIG = mPSTxSimulationChannels.Add( settings->mTRIGChannel, mSimulationSampleRateHz, Invert( mSettings->mTRIGActiveState ) );
    else
        mTRIG = NULL;

    mPSTxSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( 10.0 ) );

    mValue = 0;
}

U32 PSTxSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate,
                                                         SimulationChannelDescriptor** simulation_channels )
{
    U64 adjusted_largest_sample_requested =
        AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

    while( mCLK->GetCurrentSampleNumber() < adjusted_largest_sample_requested )
    {
        CreatePSTxData();
    }

    *simulation_channels = mPSTxSimulationChannels.GetArray();
    return mPSTxSimulationChannels.GetCount();
}

static bool valid[] =
{
	true,
	true,
	true,
	false,
	false,
	false,
	false,
	false,
	true,
	true,
	true,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	false,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
};

void PSTxSimulationDataGenerator::CreatePSTxData()
{
    while( !valid[ mValue & 0x1f ] )
        mValue++;

    mPST0->TransitionIfNeeded( mValue & ( 1 << 0 ) ? BIT_HIGH : BIT_LOW );
    mPST1->TransitionIfNeeded( mValue & ( 1 << 1 ) ? BIT_HIGH : BIT_LOW );
    mPST2->TransitionIfNeeded( mValue & ( 1 << 2 ) ? BIT_HIGH : BIT_LOW );
    mPST3->TransitionIfNeeded( mValue & ( 1 << 3 ) ? BIT_HIGH : BIT_LOW );
    mPST4->TransitionIfNeeded( mValue & ( 1 << 4 ) ? BIT_HIGH : BIT_LOW );

    if( mTRIG != NULL )
        mTRIG->TransitionIfNeeded( mValue & ( 1 << 10 ) ? BIT_HIGH : BIT_LOW );

    mPSTxSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( .5 ) );
    mCLK->Transition();

    mPSTxSimulationChannels.AdvanceAll( mClockGenerator.AdvanceByHalfPeriod( .5 ) );
    mCLK->Transition();

    mValue++;
}
