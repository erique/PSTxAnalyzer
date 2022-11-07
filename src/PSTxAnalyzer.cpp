#include "PSTxAnalyzer.h"
#include "PSTxAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

PSTxAnalyzer::PSTxAnalyzer()
:	Analyzer2(),  
	mSettings( new PSTxAnalyzerSettings() ),
	mSimulationInitilized( false )
{
    SetAnalyzerSettings( mSettings.get() );
}

PSTxAnalyzer::~PSTxAnalyzer()
{
    KillThread();
}

void PSTxAnalyzer::SetupResults()
{
    mResults.reset( new PSTxAnalyzerResults( this, mSettings.get() ) );
    SetAnalyzerResults( mResults.get() );
    mResults->AddChannelBubblesWillAppearOn( mSettings->mPST0Channel );
}

void PSTxAnalyzer::WorkerThread()
{
    mPST0 = GetAnalyzerChannelData( mSettings->mPST0Channel );
    mPST1 = GetAnalyzerChannelData( mSettings->mPST1Channel );
    mPST2 = GetAnalyzerChannelData( mSettings->mPST2Channel );
    mPST3 = GetAnalyzerChannelData( mSettings->mPST3Channel );
    mPST4 = GetAnalyzerChannelData( mSettings->mPST4Channel );
    mCLK = GetAnalyzerChannelData( mSettings->mCLKChannel );

    if( mSettings->mTRIGChannel != UNDEFINED_CHANNEL )
        mTRIG = GetAnalyzerChannelData( mSettings->mTRIGChannel );
    else
        mTRIG = NULL;

    if( mTRIG != NULL )
    {
        if( mTRIG->GetBitState() != mSettings->mTRIGActiveState )
        {
            mTRIG->AdvanceToNextEdge();
        }
        else
        {
            mTRIG->AdvanceToNextEdge();
            mTRIG->AdvanceToNextEdge();
        }
        mCLK->AdvanceToAbsPosition( mTRIG->GetSampleNumber() );
    }

    if( mCLK->GetBitState() == BIT_HIGH )
        mCLK->AdvanceToNextEdge();

    for( ;; )
    {
        U64 falling = mCLK->GetSampleNumber();

        // this is where we sample the PSTx
        mCLK->AdvanceToNextEdge(); // rising
        U64 rising = mCLK->GetSampleNumber();

        if( mTRIG )
        {
            mTRIG->AdvanceToAbsPosition( rising );
            if( mTRIG->GetBitState() != mSettings->mTRIGActiveState )
            {
                // we've fallen off the enable; fast forward
                mTRIG->AdvanceToNextEdge();
                mCLK->AdvanceToAbsPosition( mTRIG->GetSampleNumber() );
                if( mCLK->GetBitState() == BIT_HIGH )
                    mCLK->AdvanceToNextEdge();

                continue;
            }
        }

        // this is where the frame stops, next one begins
        mCLK->AdvanceToNextEdge(); // falling
        U64 fallingNext = mCLK->GetSampleNumber();

        mResults->AddMarker( rising, AnalyzerResults::Dot, mSettings->mCLKChannel );

        mPST0->AdvanceToAbsPosition( rising );
        mPST1->AdvanceToAbsPosition( rising );
        mPST2->AdvanceToAbsPosition( rising );
        mPST3->AdvanceToAbsPosition( rising );
        mPST4->AdvanceToAbsPosition( rising );

        U8 pstx = ( mPST0->GetBitState() == BIT_HIGH ? 0x01 << 0 : 0x00 ) |
                  ( mPST1->GetBitState() == BIT_HIGH ? 0x01 << 1 : 0x00 ) |
                  ( mPST2->GetBitState() == BIT_HIGH ? 0x01 << 2 : 0x00 ) |
                  ( mPST3->GetBitState() == BIT_HIGH ? 0x01 << 3 : 0x00 ) |
                  ( mPST4->GetBitState() == BIT_HIGH ? 0x01 << 4 : 0x00 );

        Frame frame;
        frame.mData1 = pstx;
        frame.mFlags = 0;
        frame.mStartingSampleInclusive = falling;
        frame.mEndingSampleInclusive = fallingNext;

        mResults->AddFrame( frame );
        mResults->CommitResults();
        ReportProgress( frame.mEndingSampleInclusive );
    }
}

bool PSTxAnalyzer::NeedsRerun()
{
    return false;
}

U32 PSTxAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate,
                                          SimulationChannelDescriptor** simulation_channels )
{
    if( mSimulationInitilized == false )
    {
        mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
        mSimulationInitilized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 PSTxAnalyzer::GetMinimumSampleRateHz()
{
    return 1 * 1000 * 1000;
}

const char* PSTxAnalyzer::GetAnalyzerName() const
{
    return "MC68060 PSTx";
}

const char* GetAnalyzerName()
{
    return "MC68060 PSTx";
}

Analyzer* CreateAnalyzer()
{
    return new PSTxAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
    delete analyzer;
}