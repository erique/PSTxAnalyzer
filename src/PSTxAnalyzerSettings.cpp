#include "PSTxAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


PSTxAnalyzerSettings::PSTxAnalyzerSettings()
    : mPST0Channel( UNDEFINED_CHANNEL ),
      mPST1Channel( UNDEFINED_CHANNEL ),
      mPST2Channel( UNDEFINED_CHANNEL ),
      mPST3Channel( UNDEFINED_CHANNEL ),
      mPST4Channel( UNDEFINED_CHANNEL ),
      mCLKChannel( UNDEFINED_CHANNEL ),
      mTRIGChannel( UNDEFINED_CHANNEL ),
      mTRIGActiveState( BIT_LOW )
{
    mPST0ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mPST0ChannelInterface->SetTitleAndTooltip( "PST0", "Instructions Completed, Bit 0" );
    mPST0ChannelInterface->SetChannel( mPST0Channel );

    mPST1ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mPST1ChannelInterface->SetTitleAndTooltip( "PST1", "Instructions Completed, Bit 1" );
    mPST1ChannelInterface->SetChannel( mPST1Channel );

    mPST2ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mPST2ChannelInterface->SetTitleAndTooltip( "PST2", "Taken Branch Instruction" );
    mPST2ChannelInterface->SetChannel( mPST2Channel );

    mPST3ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mPST3ChannelInterface->SetTitleAndTooltip( "PST3", "Branch Instruction" );
    mPST3ChannelInterface->SetChannel( mPST3Channel );

    mPST4ChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mPST4ChannelInterface->SetTitleAndTooltip( "PST4", "Supervisor Mode" );
    mPST4ChannelInterface->SetChannel( mPST4Channel );

    mCLKChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mCLKChannelInterface->SetTitleAndTooltip( "CLK", "CPU Main Clock" );
    mCLKChannelInterface->SetChannel( mCLKChannel );

    mTRIGChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
    mTRIGChannelInterface->SetTitleAndTooltip( "TRIG", "Trigger/Enable" );
    mTRIGChannelInterface->SetChannel( mTRIGChannel );
    mTRIGChannelInterface->SetSelectionOfNoneIsAllowed( true );

    mTRIGActiveStateInterface.reset( new AnalyzerSettingInterfaceNumberList() );
    mTRIGActiveStateInterface->SetTitleAndTooltip( "Trigger/Enable", "" );
    mTRIGActiveStateInterface->AddNumber( BIT_LOW, "Trigger/Enable is Active Low", "" );
    mTRIGActiveStateInterface->AddNumber( BIT_HIGH, "Trigger/Enable is Active High", "" );
    mTRIGActiveStateInterface->SetNumber( mTRIGActiveState );

    AddInterface( mPST0ChannelInterface.get() );
    AddInterface( mPST1ChannelInterface.get() );
    AddInterface( mPST2ChannelInterface.get() );
    AddInterface( mPST3ChannelInterface.get() );
    AddInterface( mPST4ChannelInterface.get() );
    AddInterface( mCLKChannelInterface.get() );
    AddInterface( mTRIGChannelInterface.get() );
    AddInterface( mTRIGActiveStateInterface.get() );

    AddExportOption( 0, "Export as text/csv file" );
    AddExportExtension( 0, "text", "txt" );
    AddExportExtension( 0, "csv", "csv" );

    ClearChannels();
    AddChannel( mPST0Channel, "MC68060 PST0", false );
    AddChannel( mPST1Channel, "MC68060 PST1", false );
    AddChannel( mPST2Channel, "MC68060 PST2", false );
    AddChannel( mPST3Channel, "MC68060 PST3", false );
    AddChannel( mPST4Channel, "MC68060 PST4", false );
    AddChannel( mCLKChannel, "MC68060 CLK", false );
    AddChannel( mTRIGChannel, "Trigger/Enable", false );
}

PSTxAnalyzerSettings::~PSTxAnalyzerSettings()
{
}

bool PSTxAnalyzerSettings::SetSettingsFromInterfaces()
{
    mPST0Channel = mPST0ChannelInterface->GetChannel();
    mPST1Channel = mPST1ChannelInterface->GetChannel();
    mPST2Channel = mPST2ChannelInterface->GetChannel();
    mPST3Channel = mPST3ChannelInterface->GetChannel();
    mPST4Channel = mPST4ChannelInterface->GetChannel();
    mCLKChannel = mCLKChannelInterface->GetChannel();
    mTRIGChannel = mTRIGChannelInterface->GetChannel();

    std::vector<Channel> channels;
    channels.push_back( mPST0Channel );
    channels.push_back( mPST1Channel );
    channels.push_back( mPST2Channel );
    channels.push_back( mPST3Channel );
    channels.push_back( mPST4Channel );
    channels.push_back( mCLKChannel );
    channels.push_back( mTRIGChannel );

    if( AnalyzerHelpers::DoChannelsOverlap( &channels[ 0 ], channels.size() ) == true )
    {
        SetErrorText( "Please select different channels for each input." );
        return false;
    }

    mTRIGActiveState = ( BitState )U32( mTRIGActiveStateInterface->GetNumber() );

    ClearChannels();
    AddChannel( mPST0Channel, "MC68060 PST0", true );
    AddChannel( mPST1Channel, "MC68060 PST1", true );
    AddChannel( mPST2Channel, "MC68060 PST2", true );
    AddChannel( mPST3Channel, "MC68060 PST3", true );
    AddChannel( mPST4Channel, "MC68060 PST4", true );
    AddChannel( mCLKChannel, "MC68060 CLK", true );
    AddChannel( mTRIGChannel, "Trigger/Enable", mTRIGChannel != UNDEFINED_CHANNEL );

    return true;
}

void PSTxAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mPST0ChannelInterface->SetChannel( mPST0Channel );
    mPST1ChannelInterface->SetChannel( mPST1Channel );
    mPST2ChannelInterface->SetChannel( mPST2Channel );
    mPST3ChannelInterface->SetChannel( mPST3Channel );
    mPST4ChannelInterface->SetChannel( mPST4Channel );
    mCLKChannelInterface->SetChannel( mCLKChannel );
    mTRIGChannelInterface->SetChannel( mTRIGChannel );
    mTRIGActiveStateInterface->SetNumber( mTRIGActiveState );
}

void PSTxAnalyzerSettings::LoadSettings( const char* settings )
{
    SimpleArchive text_archive;
    text_archive.SetString( settings );

    text_archive >> mPST0Channel;
    text_archive >> mPST1Channel;
    text_archive >> mPST2Channel;
    text_archive >> mPST3Channel;
    text_archive >> mPST4Channel;
    text_archive >> mCLKChannel;
    text_archive >> mTRIGChannel;
    text_archive >> *( U32* )&mTRIGActiveState;

    ClearChannels();
    AddChannel( mPST0Channel, "MC68060 PST0", true );
    AddChannel( mPST1Channel, "MC68060 PST1", true );
    AddChannel( mPST2Channel, "MC68060 PST2", true );
    AddChannel( mPST3Channel, "MC68060 PST3", true );
    AddChannel( mPST4Channel, "MC68060 PST4", true );
    AddChannel( mCLKChannel, "MC68060 CLK", true );
    AddChannel( mTRIGChannel, "Trigger/Enable", mTRIGChannel != UNDEFINED_CHANNEL );

    UpdateInterfacesFromSettings();
}

const char* PSTxAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << mPST0Channel;
    text_archive << mPST1Channel;
    text_archive << mPST2Channel;
    text_archive << mPST3Channel;
    text_archive << mPST4Channel;
    text_archive << mCLKChannel;
    text_archive << mTRIGChannel;
    text_archive << mTRIGActiveState;

    return SetReturnString( text_archive.GetString() );
}
