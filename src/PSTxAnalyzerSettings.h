#ifndef PSTX_ANALYZER_SETTINGS
#define PSTX_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class PSTxAnalyzerSettings : public AnalyzerSettings
{
  public:
    PSTxAnalyzerSettings();
    virtual ~PSTxAnalyzerSettings();

    virtual bool SetSettingsFromInterfaces();
    void UpdateInterfacesFromSettings();
    virtual void LoadSettings( const char* settings );
    virtual const char* SaveSettings();


    Channel mPST0Channel;
    Channel mPST1Channel;
    Channel mPST2Channel;
    Channel mPST3Channel;
    Channel mPST4Channel;
    Channel mCLKChannel;
    Channel mTRIGChannel;
    BitState mTRIGActiveState;

  protected:
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mPST0ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mPST1ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mPST2ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mPST3ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mPST4ChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mCLKChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceChannel> mTRIGChannelInterface;
    std::auto_ptr<AnalyzerSettingInterfaceNumberList> mTRIGActiveStateInterface;
};

#endif //PSTX_ANALYZER_SETTINGS
