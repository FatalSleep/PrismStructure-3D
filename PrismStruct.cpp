#pragma once
#define Export extern "C" _declspec( dllexport )
#include <fstream>
#include <unordered_map>
#include <string>

typedef unsigned char ubyte;
typedef signed char sbyte;
typedef unsigned short uint16;
typedef signed short sint16;
typedef unsigned int uint32;
typedef signed int sint32;
typedef unsigned long long int uint64;
typedef signed long long int sint64;
typedef float float32;
typedef double float64;
typedef bool boolflag;
typedef const char cbyte;

ubyte* BytePrism = nullptr;
uint64 ByteAddress = 0;
sint32 ByteBuffWidth = 0;
sint32 ByteBuffHeight = 0;
sint32 ByteBuffDepth = 0;
sint32 PrismCell2d = 0;
std::unordered_map<uint64,uint64> ByteMemory;
std::unordered_map<uint64,sint32> ByteWidth;
std::unordered_map<uint64,sint32> ByteHeight;
std::unordered_map<uint64,sint32> ByteDepth;

boolflag FileExists( cbyte* Directory ) {
	std::ifstream File( Directory , std::fstream::in );

	if ( File.is_open() == true ) {
		File.close();
		return true;
	};


	return false;
};

ubyte ByteFromInt64( uint64 Int64 , uint16 Index ) {
	ubyte Byte = 0;

	for( ubyte i = 0; i < 8; i ++ ) {
		Byte = Byte << 1;
		Byte += ( ( ubyte ) ( ( Int64 >> ( ( Index * 8 ) + ( 7 - i ) ) ) & 1 ) );
	};

	return Byte;
};

Export float64 PrismCreate( float64 Width , float64 Height , float64 Depth ) {
	sint32 Volume = ( ( sint32 ) ( Width * Height * Depth ) );
	ubyte* Prism = new ubyte[ Volume ]();
	uint64 TempAddr = ( ( uint64 ) Prism );
	ByteMemory[ TempAddr ] = TempAddr;
	ByteWidth[ TempAddr ] = ( ( sint32 ) Width );
	ByteHeight[ TempAddr ] = ( ( sint32 ) Height );
	ByteDepth[ TempAddr ] = ( ( sint32 ) Depth );
	return ( ( float64 ) TempAddr );
};

Export float64 PrismSave( float64 Address , cbyte* Directory ) {
	uint64 TempAddr = ( ( uint64 ) Address );

	if ( ByteMemory.find( TempAddr ) != ByteMemory.end() ) {
		ubyte* TempPrism = ( ( ubyte* ) TempAddr );
		sint32 TempWidth = ByteWidth.at( TempAddr );
		sint32 TempHeight = ByteHeight.at( TempAddr );
		sint32 TempDepth = ByteDepth.at( TempAddr );
		sint32 TempSize = TempWidth * TempHeight * TempDepth;
		
		sint32 CompSize = ( TempSize % 8 == 0 ) ? TempSize >> 3 : ( TempSize >> 3 ) + ( 8 - ( TempSize % 8 ) );
		uint64* CompData = new uint64[ CompSize ];

		std::fstream File;
	
		if ( FileExists( Directory ) == false ) {
			File.open( Directory , std::fstream::out );
		} else {
			File.open( Directory , std::fstream::out | std::fstream::trunc );
		};

		File << "[PrismChunk]" << std::endl;
		File << ( ( uint64 ) CompSize ) << std::endl;
		File << ( ( uint64 ) TempWidth ) << std::endl;
		File << ( ( uint64 ) TempHeight ) << std::endl;
		File << ( ( uint64 ) TempDepth ) << std::endl;
		
		for( sint32 CompCell = 0; CompCell < CompSize; CompCell ++ ) {
			for( sint32 byte = 0; byte < 8; byte ++ ) {
				if ( ( CompCell << 3 ) + byte < TempSize ) {
					CompData[ CompCell ] = ( CompData[ CompCell ] << 8 ) + TempPrism[ ( CompCell * 8 ) + byte ];
				};
			};

			File << CompData[ CompCell ] << std::endl;
		};

		File.close();
		delete[] CompData;
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismLoad( cbyte* Directory ) {
	uint64* PrismData = new uint64[ 5 ];
	std::fstream File;


	if ( PrismData[ 0 ] != 0 && FileExists( Directory ) == true ) {
		File.open( Directory , std::fstream::in );
		std::string Header = "";
		File >> Header;

		if ( Header == "[PrismChunk]" ) {
			sint32 CompSize , TempWidth , TempHeight , TempDepth;
			File >> CompSize;
			File >> TempWidth;
			File >> TempHeight;
			File >> TempDepth;
			sint32 TempSize = ( ( sint32 ) ( TempWidth * TempHeight * TempDepth ) );
			ubyte* TempPrism = new ubyte[ TempSize ];
			
			uint64 TempCell;
			for( sint32 cell = 0; cell < CompSize; cell ++ ) {
				File >> TempCell;

				for( sint32 byte = 7; byte > -1; byte -- ) {
					if ( ( cell * 8 ) + ( 7 - byte ) < TempSize ) {
						TempPrism[ ( cell * 8 ) + ( 7 - byte ) ] = ByteFromInt64( TempCell , byte );
					};
				};
			};
			
			uint64 TempAddr = ( ( uint64 ) TempPrism );
			ByteMemory[ TempAddr ] = TempAddr;
			ByteWidth[ TempAddr ] = TempWidth;
			ByteHeight[ TempAddr ] = TempHeight;
			ByteDepth[ TempAddr ] = TempDepth;
			
			delete[] PrismData;
			return ( ( float64 ) ( ( uint64 ) TempPrism ) );
		};
	};

	delete[] PrismData;
	return -1.0;
};

Export float64 PrismAllocated( void ) {
    return ByteMemory.size();
};

Export float64 PrismDelete( float64 Address ) {
	uint64 TempAddr = ( ( uint64 ) Address );

	if ( ByteMemory.find( TempAddr ) != ByteMemory.end() ) {
		ByteMemory.erase( TempAddr );
		ByteWidth.erase( TempAddr );
		ByteHeight.erase( TempAddr );
		ByteDepth.erase( TempAddr );
		
		if ( TempAddr == ( ( uint64 ) BytePrism ) ) {
			BytePrism = nullptr;
			ByteAddress = 0;
			ByteBuffWidth = 0;
			ByteBuffHeight = 0;
			ByteBuffDepth = 0;
			PrismCell2d = 0;
		};
		
		delete[] ( ( ubyte* ) TempAddr );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismExists( float64 Address ) {
	uint64 TempAddr = ( ( uint64 ) Address );

	if ( ByteMemory.find( TempAddr ) != ByteMemory.end() ) {
		return 1.0;
	};
	
	return -1.0;
};

Export float64 PrismTarget( float64 Address ) {
	uint64 TempAddr = ( ( uint64 ) Address );

	if ( ByteMemory.find( TempAddr ) != ByteMemory.end() ) {
		ByteBuffWidth = ByteWidth.at( TempAddr );
		ByteBuffHeight = ByteHeight.at( TempAddr );
		ByteBuffDepth = ByteDepth.at( TempAddr );
		PrismCell2d = ( ByteBuffWidth * ByteBuffHeight );
		BytePrism = ( ( ubyte* ) TempAddr );
		ByteAddress = TempAddr;
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismReset( void ) {
	BytePrism = nullptr;
	ByteAddress = 0;
	ByteBuffWidth = 0;
	ByteBuffHeight = 0;
	ByteBuffDepth = 0;
	PrismCell2d = 0;
	return 1.0;
};

Export float64 PrismHasTarget( void ) {
	if ( BytePrism == nullptr ) {
		return 1.0;
	} else {
		return -1.0;
	};
};

Export float64 PrismAddress( void ) {
	return ( ( float64 ) ByteAddress );
};

Export float64 PrismDeallocate( void ) {
	BytePrism = nullptr;
	ByteAddress = 0;
	ByteBuffWidth = 0;
	ByteBuffHeight = 0;
	ByteBuffDepth = 0;
	PrismCell2d = 0;
	uint64 Address = 0;
	float64 Dealloc = 0;

	if ( ByteMemory.empty() == false ) {
		while( ByteMemory.size() > 0 ) {
			uint64 TempAddr = ByteMemory.at( ByteMemory.begin()->first );
			ByteMemory.erase( ByteMemory.begin() );
			delete[] ( ( ubyte* ) TempAddr );
			Dealloc += 1.0;
		};
	};

	ByteMemory.clear();
	ByteWidth.clear();
	ByteHeight.clear();
	ByteDepth.clear();
	return Dealloc;
};

Export float64 PrismCells( void ) {
	if ( BytePrism == nullptr ) {
		return -1.0;
	};

	return ( ( float64 ) ( PrismCell2d * ByteBuffDepth ) );
};

Export float64 PrismWidth( void ) {
	if ( BytePrism == nullptr ) {
		return -1.0;
	};

	return ( ( float64 ) ByteBuffWidth );
};

Export float64 PrismHeight( void ) {
	if ( BytePrism == nullptr ) {
		return -1.0;
	};

	return ( ( float64 ) ByteBuffHeight );
};

Export float64 PrismDepth( void ) {
	if ( BytePrism == nullptr ) {
		return -1.0;
	};

	return ( ( float64 ) ByteBuffDepth );
};

Export float64 PrismCopy( float64 SrceAddr , float64 DestAddr ) {
	uint64 TempSrceAddr = ( ( uint64 ) SrceAddr );
	uint64 TempDestAddr = ( ( uint64 ) DestAddr );
	boolflag SrceExists = false;
	boolflag DestExists = false;
	float64 Cells = 0;
	sint32 SrceWidth = 0;
	sint32 SrceHeight = 0;
	sint32 SrceDepth = 0;
	sint32 DestWidth = 0;
	sint32 DestHeight = 0;
	sint32 DestDepth = 0;

	if ( ByteMemory.find( TempSrceAddr ) != ByteMemory.end() ) {
		SrceExists = true;
		SrceWidth = ByteWidth.at( TempSrceAddr );
		SrceHeight = ByteHeight.at( TempSrceAddr );
		SrceDepth = ByteDepth.at( TempSrceAddr );
	};

	if ( ByteMemory.find( TempDestAddr ) != ByteMemory.end() ) {
		DestExists = true;
		DestWidth = ByteWidth.at( TempDestAddr );
		DestHeight = ByteHeight.at( TempDestAddr );
		DestDepth = ByteDepth.at( TempDestAddr );
	};
	
	if (  SrceExists && DestExists ) {
		ubyte* SrcePrism = ( ( ubyte* ) TempSrceAddr );
		ubyte* DestPrism = ( ( ubyte* ) TempDestAddr );
		
		for( sint32 w = 0; w < SrceWidth; w ++ ) {
			for( sint32 h = 0; h < SrceWidth; h ++ ) {
				for( sint32 d = 0; d < SrceWidth; d ++ ) {
					if ( w < DestWidth && h < DestHeight && d < DestDepth ) {
						DestPrism[ ( ( DestWidth * DestHeight ) * d ) + ( DestWidth * h ) + w ] = SrcePrism[ ( ( DestWidth * DestHeight ) * d ) + ( DestWidth * h ) + w ];
						Cells ++;
					};
				};
			};
		};
		
		return Cells;
	} else {
		return -1.0;
	};
};

Export float64 PrismCopyExt( float64 SrceAddr , float64 DestAddr , float64 SrceX , float64 SrceY , float64 SrceZ , float64 DestX , float64 DestY , float64 DestZ , float64 ChunkW , float64 ChunkH , float64 ChunkD ) {
	uint64 TempSrceAddr = ( ( uint64 ) SrceAddr );
	uint64 TempDestAddr = ( ( uint64 ) DestAddr );
	boolflag SrceExists = false;
	boolflag DestExists = false;
	float64 Cells = 0;
	sint32 SrceWidth = 0;
	sint32 SrceHeight = 0;
	sint32 SrceDepth = 0;
	sint32 DestWidth = 0;
	sint32 DestHeight = 0;
	sint32 DestDepth = 0;

	if ( ByteMemory.find( TempSrceAddr ) != ByteMemory.end() ) {
		SrceExists = true;
		SrceWidth = ByteWidth.at( TempSrceAddr );
		SrceHeight = ByteHeight.at( TempSrceAddr );
		SrceDepth = ByteDepth.at( TempSrceAddr );
	};

	if ( ByteMemory.find( TempDestAddr ) != ByteMemory.end() ) {
		DestExists = true;
		DestWidth = ByteWidth.at( TempDestAddr );
		DestHeight = ByteHeight.at( TempDestAddr );
		DestDepth = ByteDepth.at( TempDestAddr );
	};
	
	if (  SrceExists == true && DestExists == true ) {
		ubyte* SrcePrism = ( ( ubyte* ) TempSrceAddr );
		ubyte* DestPrism = ( ( ubyte* ) TempDestAddr );
		
		for( sint32 w = SrceX; w < SrceX + ChunkW; w ++ ) {
			for( sint32 h = SrceY; h < SrceY + ChunkH; h ++ ) {
				for( sint32 d = SrceZ; d < SrceZ + ChunkD; d ++ ) {
					if ( w >= 0 && h >= 0 && d >= 0 && w < SrceWidth && h < SrceHeight && d < SrceDepth && w < DestWidth && h < DestHeight && d < DestDepth ) {
						DestPrism[ ( ( uint32 ) ( ( ( DestWidth * DestHeight ) * ( DestZ + ( d - SrceZ ) ) ) + ( DestWidth * ( DestY + ( h - SrceY ) ) ) + ( DestX + ( w - SrceX ) ) ) ) ] = SrcePrism[ ( ( uint64 ) ( ( DestWidth * DestHeight ) * d ) + ( DestWidth * h ) + w ) ];
						Cells ++;
					};
				};
			};
		};
		
		return Cells;
	} else {
		return -1.0;
	};
};

Export float64 PrismGetWrap( float64 Xpos , float64 Ypos , float64 Zpos ) {
	if ( BytePrism == nullptr ) { return -1.0; }

	sint32 WrapXpos = Xpos;
	sint32 WrapYpos = Ypos;
	sint32 WrapZpos = Zpos;

	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		return BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ];
	} else {
		WrapXpos = ( WrapXpos < 0 ) ? ByteBuffWidth - ( WrapXpos % ( 0 - ByteBuffWidth ) ) : WrapXpos % ByteBuffWidth;
		WrapYpos = ( WrapYpos < 0 ) ? ByteBuffHeight - ( WrapYpos % ( 0 - ByteBuffHeight ) ) : WrapYpos % ByteBuffHeight;
		WrapZpos = ( WrapZpos < 0 ) ? ByteBuffDepth - ( WrapZpos % ( 0 - ByteBuffDepth ) ) : WrapZpos % ByteBuffDepth;
		return BytePrism[ ( ( uint32 ) ( ( PrismCell2d * WrapZpos ) + ( ByteBuffWidth * WrapYpos ) + WrapXpos ) ) ];
	};

	return -1.0;
};

Export float64 PrismSetWrap( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	
	sint32 WrapXpos = Xpos;
	sint32 WrapYpos = Ypos;
	sint32 WrapZpos = Zpos;

	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] = ( ( ubyte ) Byte );
		return 1.0;
	} else {
		WrapXpos = ( WrapXpos < 0 ) ? ByteBuffWidth - ( WrapXpos % ( 0 - ByteBuffWidth ) ) : WrapXpos % ByteBuffWidth;
		WrapYpos = ( WrapYpos < 0 ) ? ByteBuffHeight - ( WrapYpos % ( 0 - ByteBuffHeight ) ) : WrapYpos % ByteBuffHeight;
		WrapZpos = ( WrapZpos < 0 ) ? ByteBuffDepth - ( WrapZpos % ( 0 - ByteBuffDepth ) ) : WrapZpos % ByteBuffDepth;
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * WrapZpos ) + ( ByteBuffWidth * WrapYpos ) + WrapXpos ) ) ] = ( ( ubyte ) Byte );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismGet( float64 Xpos , float64 Ypos , float64 Zpos ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		return BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ];
	};

	return -1.0;
};

Export float64 PrismSet( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] = ( ( ubyte ) Byte );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismAdd( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] += ( ( ubyte ) Byte );
		return 1.0;
	};
	
	return -1.0;
};

Export float64 PrismSubtract( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] -= ( ( ubyte ) Byte );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismMultiply( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] *= ( ( ubyte ) Byte );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismDivide( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] /= ( ( ubyte ) Byte );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismModulo( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] %= ( ( ubyte ) Byte );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismBitShift( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte , float64 Shift ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	ubyte Value = BytePrism[ ( ( uint64 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ];

	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		if ( Shift >= 1.0 ) {
			BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] = Value >> ( ( ubyte ) Byte );
			return 1.0;
		} else {
			BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] = Value << ( ( ubyte ) Byte );
			return 1.0;
		};
	};

	return -1.0;
};

Export float64 PrismBitAnd( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] &= ( ( ubyte ) Byte );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismBitOr( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] |= ( ( ubyte ) Byte );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismBitXor( float64 Xpos , float64 Ypos , float64 Zpos , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	if ( Xpos < ByteBuffWidth && Ypos < ByteBuffHeight && Zpos < ByteBuffDepth && Xpos >= 0 && Ypos >= 0 && Zpos >= 0 ) {
		BytePrism[ ( ( uint32 ) ( ( PrismCell2d * Zpos ) + ( ByteBuffWidth * Ypos ) + Xpos ) ) ] ^= ( ( ubyte ) Byte );
		return 1.0;
	};

	return -1.0;
};

Export float64 PrismAreaSet( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte ) {	
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Cells = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] = ( ( ubyte ) Byte );
					Cells ++;
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismAreaAdd( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Cells = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] += ( ( ubyte ) Byte );
					Cells ++;
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismAreaSubtract( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Cells = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] -= ( ( ubyte ) Byte );
					Cells ++;
				};
			};
		};
	};
	
	return Cells;
};

Export float64 PrismAreaMultiply( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Cells = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] *= ( ( ubyte ) Byte );
					Cells ++;
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismAreaDivide( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Cells = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] /= ( ( ubyte ) Byte );
					Cells ++;
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismAreaModulo( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Cells = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] %= ( ( ubyte ) Byte );
					Cells ++;
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismAreaBitshift( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte , float64 Shift ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	ubyte Value = 0;
	float64 Cells = 0;

	if ( Shift >= 1.0 ) {
		for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
			for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
				for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
					if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
						Value = BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ];
						BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] = Value >> ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	} else {
		for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
			for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
				for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
					if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
						Value = BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ];
						BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] = Value << ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismAreaBitAnd( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Cells = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] &= ( ( ubyte ) Byte );
					Cells ++;
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismAreaBitOr( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Cells = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] |= ( ( ubyte ) Byte );
					Cells ++;
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismAreaBitXor( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Cells = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ] ^= ( ( ubyte ) Byte );
					Cells ++;
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismAreaSum( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Sum = 0;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					Sum += BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ];
				};
			};
		};
	};

	return Sum;
};

Export float64 PrismAreaMin( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Min = 255;
	uint32 Cell;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				Cell = ( ( uint32 ) ( ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ) );

				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					if ( BytePrism[ Cell ] < Min ) {
						Min = BytePrism[ Cell ];
					};
				};
			};
		};
	};

	return Min;
};

Export float64 PrismAreaMax( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Max = 0;
	uint32 Cell;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				Cell = ( ( uint32 ) ( ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ) );\

				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					if ( BytePrism[ Cell ] > Max ) {
						Max = BytePrism[ Cell ];
					};
				};
			};
		};
	};

	return Max;
};

Export float64 PrismAreaMean( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Mean = 0;
	uint32 Cell;

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					Cell = ( ( uint32 ) ( ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ) );

					if ( BytePrism[ Cell ] < Mean ) {
						Mean += BytePrism[ ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ];
					};
				};
			};
		};
	};

	return Mean / ( ( float64 ) ( Width * Height * Depth ) );
};

Export float64 PrismAreaRange( float64 Xpos , float64 Ypos , float64 Zpos , float64 Width , float64 Height , float64 Depth , float64 RngMin , float64 RngMax ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 Mean = 0;
	uint32 Cell;
	ubyte Value;
	ubyte Min = ( ( ubyte ) RngMin );
	ubyte Max = ( ( ubyte ) RngMax );

	for( sint32 w = Xpos; w < Xpos + Width; w ++ ) {
		for( sint32 h = Ypos; h < Ypos + Height; h ++ ) {
			for( sint32 d = Zpos; d < Zpos + Depth; d ++ ) {
				if ( w < ByteBuffWidth && h < ByteBuffHeight && d < ByteBuffDepth && w >= 0 && h >= 0 && d >= 0 ) {
					Cell = ( ( uint32 ) ( ( PrismCell2d * d ) + ( ByteBuffWidth * h ) + w ) );
					Value = BytePrism[ Cell ];

					if ( BytePrism[ Cell ] < Mean ) {
						if ( Cell < Min || Cell > Max ) {
							return 0.0;
						};
					};
				};
			};
		};
	};

	return 1.0;
};

Export float64 PrismSphereSet( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] = ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereAdd( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] += ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereSubtract( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] -= ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereMultiply( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] *= ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereDivide( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] *= ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereModulo( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] %= ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereBitshift( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte , float64 Shift ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	ubyte Value = 0;
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	if ( Shift >= 1.0 ) {
		for( sint32 w = -Radius; w <= Radius; w ++ ) {
			for( sint32 h = -Radius; h <= Radius; h ++ ) {
				for( sint32 d = -Radius; d <= Radius; d ++ ) {
					if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
						if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
							Value = BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ];
							BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] = Value >> ( ( ubyte ) Byte );
							Cells ++;
						};
					};
				};
			};
		};
	} else {
		for( sint32 w = -Radius; w <= Radius; w ++ ) {
			for( sint32 h = -Radius; h <= Radius; h ++ ) {
				for( sint32 d = -Radius; d <= Radius; d ++ ) {
					if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
						if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
							Value = BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ];
							BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] = Value << ( ( ubyte ) Byte );
							Cells ++;
						};
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereBitAnd( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] &= ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereBitOr( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] |= ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereBitXor( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 Byte ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	float64 RadSquared = Radius * Radius;
	float64 Cells = 0;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ] |= ( ( ubyte ) Byte );
						Cells ++;
					};
				};
			};
		};
	};

	return Cells;
};

Export float64 PrismSphereSum( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	uint64 Sum = 0;
	float64 RadSquared = Radius * Radius;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						Sum += BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ];
					};
				};
			};
		};
	};

	return ( ( float64 ) Sum );
};

Export float64 PrismSphereMin( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	uint64 Min = 255;
	float64 RadSquared = Radius * Radius;
	uint32 Cell;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						Cell = ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) );

						if ( BytePrism[ Cell ] < Min ) {
							Min = BytePrism[ Cell ];
						};
					};
				};
			};
		};
	};

	return ( ( float64 ) Min );
};

Export float64 PrismSphereMax( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	uint64 Max = 0;
	float64 RadSquared = Radius * Radius;
	uint32 Cell;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						Cell = ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) );
						
						if ( BytePrism[ Cell ] > Max ) {
							Max = BytePrism[ Cell ];
						};
					};
				};
			};
		};
	};

	return ( ( float64 ) Max );
};

Export float64 PrismSphereMean( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	uint64 Mean = 255;
	float64 Cells = 0;
	float64 RadSquared = Radius * Radius;

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						Mean += BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ];
						Cells ++;
					};
				};
			};
		};
	};

	return ( ( float64 ) ( Mean / Cells ) );
};

Export float64 PrismSphereRange( float64 Xpos , float64 Ypos , float64 Zpos , float64 Radius , float64 RngMin , float64 RngMax ) {
	if ( BytePrism == nullptr ) { return -1.0; }
	uint64 Mean = 255;
	uint32 Cell = 0;
	float64 RadSquared = Radius * Radius;
	ubyte Min = ( ( ubyte ) RngMin );
	ubyte Max = ( ( ubyte ) RngMax );

	for( sint32 w = -Radius; w <= Radius; w ++ ) {
		for( sint32 h = -Radius; h <= Radius; h ++ ) {
			for( sint32 d = -Radius; d <= Radius; d ++ ) {
				if ( Xpos + w < ByteBuffWidth && Ypos + h < ByteBuffHeight && Zpos + d < ByteBuffDepth && Xpos + w >= 0 && Ypos + h >= 0 && Zpos + d >= 0 ) {
					if ( ( w * w ) + ( h * h ) + ( d * d ) <= RadSquared ) {
						Cell = BytePrism[ ( ( uint32 ) ( ( PrismCell2d * ( Zpos + d ) ) + ( ByteBuffWidth * ( Ypos + h ) ) + ( Xpos + w ) ) ) ];

						if ( Cell < Min || Cell > Max ) {
							return 0.0;
						};
					};
				};
			};
		};
	};

	return 1.0;
};
