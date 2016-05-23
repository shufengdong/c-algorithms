#ifndef IEEEFORMAT_DATA_H
#define IEEEFORMAT_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#define BUS_CARD_BEGIN	"BUS DATA FOLLOWS"
#define BUS_CARD_END	"-999"

#define BRANCH_CARD_BEGIN	"BRANCH DATA FOLLOWS"
#define BRANCH_CARD_END		"-999"

#define LOSS_ZONE_CARD_BEGIN "LOSS ZONES FOLLOWS"
#define LOSS_ZONE_CARD_END "-99"

#define INTERCHANGE_CARD_BEGIN "INTERCHANGE DATA FOLLOWS"
#define INTERCHANGE_CARD_END "-9"

#define TIE_LINE_CARD_BEGIN "TIE LINES FOLLOWS"
#define TIE_LINE_CARD_END "-999"

typedef struct _TitleData TitleData;

typedef struct _BusData BusData;

typedef struct _BranchData BranchData;

typedef struct _LossZoneData LossZoneData;

typedef struct _InterChangeData InterChangeData;

typedef struct _TieLineData TieLineData;

typedef struct _IEEEDataIsland IEEEDataIsland;

struct _TitleData {
    char date[9]; //format: DD/MM/YY
    char originatorName[21];
    double mvaBase;
    int year;
    char season;
    char caseId[29];
};

/**
 * Definition of an @ref BusData.
 */

struct _BusData {

	int busNum;

    char name[13];
    int area;
    int lossZone;
    int busType;
    double finalVolt;
    double finalAngle;
    double loadP;
    double loadQ;
    double genP;
    double genQ;
    double voltBase;
    double desireV;
    double maximum;
    double minimum;
    double G;
    double B;
    int remoteControlBus;
};

struct _BranchData {
    int tapBusNumber;
    int zBusNumber;
    int area;
    int lossZone;
    int circuit;
    int type;
    double R;
    double X;
    double lineB;
    int mvaRating1;
    int mvaRating2;
    int mvaRating3;
    int controlBusNumber;
    int side;
    double transRatio;
    double transAngle;
    double minimumTap;
    double maximumTap;
    double stepSize;
    double minimum;
    double maximum;
};

struct _LossZoneData {
    int lossZoneNumber;
    char lossZoneName[13];
};

struct _InterChangeData {
    int areaNumber; //no zeros!
    int slackBusNumber;
    char alternateSwingBusName[13];
    double areaExport; //area interchange export, MW
    double areaTolerance; //area interchange tolerance, MW
    char areaCode[7];
    char areaName[31];
};

struct _TieLineData {
    int meteredBusNum;
    int meteredAreaNum;
    int nonMeteredBusNum;
    int nonMeteredAreaNum;
    int circuitNum;
};

struct _IEEEDataIsland {
    TitleData title;
    BusData * buses;
    BranchData * branches;
    LossZoneData * lossZones;
    TieLineData * tieLines;
    InterChangeData * interChanges;

    int busN;
    int branchN;
    int lossZoneN;
    int tieLineN;
    int interchangeN;
    double mvaBase;
};

void parse_title_data(TitleData * t, char * strLine);

void write_title_data(TitleData * t, char * buffer);

void parse_bus_data(BusData * bus, char * strLine);

void write_bus_data(BusData * bus, char * buffer);

void parse_branch_data(BranchData * branch, char * strLine);

void write_branch_data(BranchData * branch, char * buffer);

void parse_loss_zone_data(LossZoneData * z, char * strLine);

void write_loss_zone_data(LossZoneData * z, char * buffer);

void parse_island_file(IEEEDataIsland * island, char * filePath);

void parse_island_content(IEEEDataIsland * island, char * content);

void write_island_file(IEEEDataIsland * island, char * filePath);

void free_island(IEEEDataIsland * island);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef IEEEFORMAT_DATA_H */
