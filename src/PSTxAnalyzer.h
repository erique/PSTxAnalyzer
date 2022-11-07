#ifndef PSTX_ANALYZER_H
#define PSTX_ANALYZER_H

#include <Analyzer.h>
#include "PSTxAnalyzerResults.h"
#include "PSTxSimulationDataGenerator.h"

class PSTxAnalyzerSettings;
class ANALYZER_EXPORT PSTxAnalyzer : public Analyzer2
{
  public:
    PSTxAnalyzer();
    virtual ~PSTxAnalyzer();

    virtual void SetupResults();
    virtual void WorkerThread();

    virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
    virtual U32 GetMinimumSampleRateHz();

    virtual const char* GetAnalyzerName() const;
    virtual bool NeedsRerun();

  protected: //vars
    std::auto_ptr<PSTxAnalyzerSettings> mSettings;
    std::auto_ptr<PSTxAnalyzerResults> mResults;

    AnalyzerChannelData* mPST0;
    AnalyzerChannelData* mPST1;
    AnalyzerChannelData* mPST2;
    AnalyzerChannelData* mPST3;
    AnalyzerChannelData* mPST4;
    AnalyzerChannelData* mCLK;
    AnalyzerChannelData* mTRIG;

    PSTxSimulationDataGenerator mSimulationDataGenerator;
    bool mSimulationInitilized;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //PSTX_ANALYZER_H
