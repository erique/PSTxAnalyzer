#include "PSTxAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "PSTxAnalyzer.h"
#include "PSTxAnalyzerSettings.h"
#include <iostream>
#include <fstream>

/*
Hex PST4 PST3 PST2 PST1 PST0 Internal Processor Status
$00   0    0    0    0    0  Continue Execution in User Mode
$01   0    0    0    0    1  Complete 1 Instruction in User Mode
$02   0    0    0    1    0  Complete 2 Instructions in User Mode
$03   0    0    0    1    1  —
$04   0    0    1    0    0  —
$05   0    0    1    0    1  —
$06   0    0    1    1    0  —
$07   0    0    1    1    1  —
$08   0    1    0    0    0  Emulator Mode Entry Exception Processing
$09   0    1    0    0    1  Complete Not Taken Branch in User Mode
$0A   0    1    0    1    0  Complete Not Taken Branch Plus 1 Instruction in User Mode
$0B   0    1    0    1    1  IED Cycle of Branch to Vector, Emulator Entry Exception
$0C   0    1    1    0    0  —
$0D   0    1    1    0    1  Complete Taken Branch in User Mode
$0E   0    1    1    1    0  Complete Taken Branch Plus 1 Instruction in User Mode
$0F   0    1    1    1    1  Complete Taken Branch Plus 2 Instructions in User Mode
$10   1    0    0    0    0  Continue Execution in Supervisor Mode
$11   1    0    0    0    1  Complete 1 Instruction in Supervisor Mode
$12   1    0    0    1    0  Complete 2 Instructions in Supervisor Mode
$13   1    0    0    1    1  —
$14   1    0    1    0    0  —
$15   1    0    1    0    1  Complete RTE Instruction in Supervisor Mode
$16   1    0    1    1    0  Low-Power Stopped State; Waiting for an Interrupt or Reset
$17   1    0    1    1    1  MC68060 Is Stopped Waiting for an Interrupt
$18   1    1    0    0    0  MC68060 Is Processing an Exception
$19   1    1    0    0    1  Complete Not Taken Branch in Supervisor Mode
$1A   1    1    0    1    0  Complete Not Taken Branch Plus 1 Instruction in Supervisor Mode
$1B   1    1    0    1    1  IED Cycle of Branch to Vector, Exception Processing
$1C   1    1    1    0    0  MC68060 Is Halted
$1D   1    1    1    0    1  Complete Taken Branch in Supervisor Mode
$1E   1    1    1    1    0  Complete Taken Branch Plus 1 Instruction in Supervisor Mode
$1F   1    1    1    1    1  Complete Taken Branch Plus 2 Instructions in Supervisor Mode
*/

static const char* status[] = {
    "Continue Execution in User Mode",
    "Complete 1 Instruction in User Mode",
    "Complete 2 Instructions in User Mode",
    0,
    0,
    0,
    0,
    0,
    "Emulator Mode Entry Exception Processing",
    "Complete Not Taken Branch in User Mode",
    "Complete Not Taken Branch Plus 1 Instruction in User Mode",
    "IED Cycle of Branch to Vector, Emulator Entry Exception",
    0,
    "Complete Taken Branch in User Mode",
    "Complete Taken Branch Plus 1 Instruction in User Mode",
    "Complete Taken Branch Plus 2 Instructions in User Mode",
    "Continue Execution in Supervisor Mode",
    "Complete 1 Instruction in Supervisor Mode",
    "Complete 2 Instructions in Supervisor Mode",
    0,
    0,
    "Complete RTE Instruction in Supervisor Mode",
    "Low-Power Stopped State; Waiting for an Interrupt or Reset",
    "MC68060 Is Stopped Waiting for an Interrupt",
    "MC68060 Is Processing an Exception",
    "Complete Not Taken Branch in Supervisor Mode",
    "Complete Not Taken Branch Plus 1 Instruction in Supervisor Mode",
    "IED Cycle of Branch to Vector, Exception Processing",
    "MC68060 Is Halted",
    "Complete Taken Branch in Supervisor Mode",
    "Complete Taken Branch Plus 1 Instruction in Supervisor Mode",
    "Complete Taken Branch Plus 2 Instructions in Supervisor Mode",
};

static const char* status_short[] = {
    "-",
    "1",
    "2",
    0,
    0,
    0,
    0,
    0,
    "Emulator Mode Entry Exception Processing",
    "!BRx",
    "!BRx+1",
    "IED Cycle of Branch to Vector, Emulator Entry Exception",
    0,
    "BRx^",
    "BRx^+1",
    "BRx^+2",
    "- (S)",
    "1 (S)",
    "2 (S)",
    0,
    0,
    "RTE (S)",
    "W8 IRQ|RST",
    "W8 IRQ",
    "Exception Processing",
    "!BRx (S)",
    "!BRx+1 (S)",
    "IED Cycle of Branch to Vector, Exception Processing",
    "HALT",
    "BRx^ (S)",
    "BRx^+1 (S)",
    "BRx^+2 (S)",
};


static uint32_t instr_count[] =
{
	0,
	1,
	2,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	2,
	0,
	0,
	1,
	2,
	3,
	0,
	1,
	2,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	2,
	0,
	0,
	1,
	2,
	3,
};



PSTxAnalyzerResults::PSTxAnalyzerResults( PSTxAnalyzer* analyzer, PSTxAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

PSTxAnalyzerResults::~PSTxAnalyzerResults()
{
}

void PSTxAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
    ClearResultStrings();
    Frame frame = GetFrame( frame_index );

    const char* str = status_short[ frame.mData1 & 0x1f ];
    if( !str )
    {
        str = "ERROR";
    }
    AddResultString( str );
}

void PSTxAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
    std::ofstream file_stream( file, std::ios::out );

    U64 trigger_sample = mAnalyzer->GetTriggerSample();
    U32 sample_rate = mAnalyzer->GetSampleRate();

    file_stream << "Time [s],CPU Cycle,PSTx,Instructions Completed,Instructions Accumulated,Description" << std::endl;

    U64 num_frames = GetNumFrames();
    U32 accum_completed = 0;
    for( U64 i = 0; i < num_frames; i++ )
    {
        Frame frame = GetFrame( i );

        U32 instr_completed = instr_count[ frame.mData1 & 0x1f ];
        accum_completed += instr_completed;

        const char* desc = status[ frame.mData1 & 0x1f ];

        char time_str[ 128 ];
        AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

        char cycle_str[ 128 ];
        AnalyzerHelpers::GetNumberString( i, Decimal, 64, cycle_str, 128 );

        char pstx_str[ 128 ];
        AnalyzerHelpers::GetNumberString( frame.mData1, Hexadecimal, 8, pstx_str, 128 );

        char instr_str[ 128 ];
        AnalyzerHelpers::GetNumberString( instr_completed, Decimal, 32, instr_str, 128 );

        char accum_str[ 128 ];
        AnalyzerHelpers::GetNumberString( accum_completed, Decimal, 32, accum_str, 128 );

        file_stream << time_str << "," << cycle_str << "," << pstx_str << "," << instr_str << "," << accum_str << "," << desc << std::endl;

        if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
        {
            file_stream.close();
            return;
        }
    }

    file_stream.close();
}

void PSTxAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
    Frame frame = GetFrame( frame_index );
    ClearTabularText();

    const char* str = status[ frame.mData1 & 0x1f ];
    if( !str )
    {
        str = "ERROR";
    }
    AddTabularText( str );

#endif
}

void PSTxAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
    //not supported
}

void PSTxAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
    //not supported
}