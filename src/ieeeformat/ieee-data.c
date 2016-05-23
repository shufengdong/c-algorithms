#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ieee-data.h"

/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

static void trim(char * s) {
    int n, i, j;
    for(n = strlen(s) - 1; n >= 0; n--) {
       if(s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
           break;
       s[n] = '\0';
    }
    i = 0;
    while(s[i] == ' ' || s[i] == '\t' || s[i] == '\n')
        i++;
    if(i < 1) return;
    for(j = 0; j <= n - i; j++)
        s[j] = s[j + i];
}

void parse_title_data(TitleData * t, char * strLine) {
    char tmp3[5];
    //解析日期
    strncpy(t->date, strLine + 1, 8);
    t->date[8] = '\0';
    trim(t->date);
    //解析作者名称
    strncpy(t->originatorName, strLine + 10, 20);
    t->originatorName[20] = '\0';
    trim(t->originatorName);
    //解析年
    sscanf(strLine + 31, "%6lf ", &t->mvaBase);
    strncpy(tmp3, strLine + 38, 4);
    tmp3[4] = '\0';
    sscanf(tmp3, "%d", &t->year);
    //解析季
    t->season = *(strLine + 43);
    //解析id
    int l = strlen(strLine) - 45;
    if(l < 0) {
        t->caseId[0] = '\0';
        return;
    }
    if(l > 28)
        l = 28;
    strncpy(t->caseId, strLine + 45, l);
    t->caseId[l] = '\0';
    trim(t->caseId);
}

void write_title_data(TitleData * t, char * buffer) {
	sprintf(buffer, " %8.8s %20.20s %6.2lf %4d %c %28.28s", t->date, t->originatorName, t->mvaBase, t->year, t->season, t->caseId);
}

void parse_bus_data(BusData * bus, char * strLine) {
    sscanf(strLine, "%4d ", &bus->busNum);
    strncpy(bus->name, strLine + 5, 12);
    bus->name[12] = '\0';
    trim(bus->name);
    sscanf(strLine + 18, "%2d%3d %2d %6lf%7lf%9lf%10lf%8lf%8lf %7lf %6lf%8lf%8lf%8lf%8lf %d",
        &bus->area, &bus->lossZone, &bus->busType, &bus->finalVolt, &bus->finalAngle, &bus->loadP, &bus->loadQ,
        &bus->genP, &bus->genQ, &bus->voltBase, &bus->desireV, &bus->maximum, &bus->minimum, &bus->G, &bus->B, &bus->remoteControlBus);
}

void write_bus_data(BusData * bus, char * buffer) {
	sprintf(buffer, "%4d %-12.12s %-2d%-3d %2d %-6.3lf%-7.2lf%-9.3lf%-10.3lf%-8.3lf%-8.3lf %7.2lf %-6.3lf%-8.1lf%-8.1lf%-8.4lf%-8.4lf %4d",
		bus->busNum, bus->name, bus->area, bus->lossZone, bus->busType, bus->finalVolt, bus->finalAngle, bus->loadP, bus->loadQ,
		bus->genP, bus->genQ, bus->voltBase, bus->desireV, bus->maximum, bus->minimum, bus->G, bus->B, bus->remoteControlBus);
}

void parse_branch_data(BranchData * b, char * strLine) {
    sscanf(strLine, "%4d %4d %2d%2d  %d %1d%10lf%11lf%10lf%5d %5d %5d %4d %d  %6lf %7lf%7lf%7lf %6lf %7lf%7lf",
            &b->tapBusNumber, &b->zBusNumber, &b->area, &b->lossZone, &b->circuit, &b->type, &b->R, &b->X, &b->lineB,
            &b->mvaRating1, &b->mvaRating2, &b->mvaRating3, &b->controlBusNumber, &b->side,
            &b->transRatio, &b->transAngle, &b->minimumTap, &b->maximumTap, &b->stepSize, &b->minimum, &b->maximum);
}

void write_branch_data(BranchData * b, char * buffer) {
    sprintf(buffer, "%4d %4d %-2d%-2d  %d %d%-10.5lf%-11.5lf%-10.5lf%-5d %-5d %-5d %4d %d  %6.3lf %-7.3lf%-7.3lf%-7.3lf %6.4lf %-7.4lf%-7.4lf",
        b->tapBusNumber, b->zBusNumber, b->area, b->lossZone, b->circuit, b->type, b->R, b->X, b->lineB,
        b->mvaRating1, b->mvaRating2, b->mvaRating3, b->controlBusNumber, b->side,
        b->transRatio, b->transAngle, b->minimumTap, b->maximumTap, b->stepSize, b->minimum, b->maximum);
}

void parse_loss_zone_data(LossZoneData * z, char * strLine) {
    sscanf(strLine, "%d %s", &z->lossZoneNumber, z->lossZoneName);
}

void write_loss_zone_data(LossZoneData * z, char * buffer) {
    sprintf(buffer, "%3d %12.12s", z->lossZoneNumber, z->lossZoneName);
}

void parse_interchange_data(InterChangeData * d, char * strLine) {
    sscanf(strLine, "%d %d %12s%8f %6f  %6s  %30s",
        &d->areaNumber, &d->slackBusNumber, d->alternateSwingBusName, &d->areaExport, &d->areaTolerance,
        &d->areaCode, &d->areaName);
}

void write_interchange_data(InterChangeData * d, char * buffer) {
    sprintf(buffer, "%2d %4d %-12.12s%-8.3lf %6.3lf  %6.6s  %30.30s",
        d->areaNumber, d->slackBusNumber, d->alternateSwingBusName, d->areaExport, d->areaTolerance,
        d->areaCode, d->areaName);
}

void parse_tie_line_data(TieLineData * tl, char * strLine) {
    sscanf(strLine, "%4d  %2d  %4d  %2d  %d",
        &tl->meteredBusNum, &tl->meteredAreaNum, &tl->nonMeteredBusNum, &tl->nonMeteredAreaNum, &tl->circuitNum);
}

void write_tie_line_data(TieLineData * tl, char * buffer) {
    sprintf(buffer, "%4d  %2d  %4d  %2d  %d",
        tl->meteredBusNum, tl->meteredAreaNum, tl->nonMeteredBusNum, tl->nonMeteredAreaNum, tl->circuitNum);
}

static int getItemN(char * strLine) {
    int result, len;
    len = strlen(strLine) - 30;
    char tmp[len + 1];
    strncpy(tmp, strLine + 30, len);
    tmp[len] = '\0';
    sscanf(tmp, "%d ", &result);
    return result;
}

static void initial_island(IEEEDataIsland * island) {
    //初始化
    island->busN = 0;
    island->branchN = 0;
    island->lossZoneN = 0;
    island->tieLineN = 0;
    island->interchangeN = 0;
    island->mvaBase = 0.0;
    island->mvaBase = 0.0;
    island->buses = NULL;
    island->branches = NULL;
    island->lossZones = NULL;
    island->tieLines = NULL;
    island->interChanges = NULL;
}

void parse_island_content(IEEEDataIsland * island, char * content) {
    const char * d = "\n";
	char * strLine;
	int n;
	//初始化
	initial_island(island);

	strLine = strtok(content, d);
	while(strlen(strLine) < 30)
        strLine = strtok(NULL, d);
    //解析Title Data
    parse_title_data(&island->title, strLine);
    island->mvaBase = island->title.mvaBase;
    printf("the MVA base is %lf MVA.\n", island->mvaBase);


    //寻找Bus数据卡开始
    n = strlen(BUS_CARD_BEGIN);
    do {
        strLine = strtok(NULL, d);
    } while(strncmp(strLine, BUS_CARD_BEGIN, n) != 0);


    int itemN = 0;
    itemN = getItemN(strLine);
    //printf("%d %s\n", itemN, strLine);
    //找到了
    island->buses = malloc(itemN * sizeof(BusData));
    island->busN = itemN;
    for(int i = 0; i < itemN; i++) {
         strLine = strtok(NULL, d);
#ifdef IEEE_FORMAT_DEBUG
        printf("%s\n", strLine);
#endif
        parse_bus_data(island->buses + i, strLine);
    }

    //寻找Branch数据卡开始
    n = strlen(BRANCH_CARD_BEGIN);
    do {
         strLine = strtok(NULL, d);
    } while(strncmp(strLine, BRANCH_CARD_BEGIN, n) != 0);

    itemN = getItemN(strLine);
    //找到了
    island->branches = malloc(itemN * sizeof(BranchData));
    island->branchN = itemN;
    //printf("%d %s\n", itemN, strLine);
    for(int i = 0; i < itemN; i++) {
         strLine = strtok(NULL, d);
#ifdef IEEE_FORMAT_DEBUG
        printf("%s\n", strLine);
#endif
        parse_branch_data(island->branches + i, strLine);
    }
    //todo：interChange, tieLine, loassZone data are not parsed!
}

void parse_island_file(IEEEDataIsland * island, char * filePath) {
    FILE * fp;
    int n;
    if((fp = fopen(filePath, "rt")) == NULL) {
        printf("Cannot open file %s!\n", filePath);
        return 0;
    }
    //初始化
    initial_island(island);
    char strLine[150];
    do {
        fgets(strLine,150, fp);
    } while(strlen(strLine) < 30);
    //解析Title Data
    parse_title_data(&island->title, strLine);
    island->mvaBase = island->title.mvaBase;
    printf("the MVA base is %lf MVA.\n", island->mvaBase);
    //寻找Bus数据卡开始
    n = strlen(BUS_CARD_BEGIN);
   	do{
   		fgets(strLine,150, fp);
   	} while(strncmp(strLine, BUS_CARD_BEGIN, n) != 0);

    int itemN = getItemN(strLine);
    //printf("%d %s\n", itemN, strLine);
    //找到了
    island->buses = malloc(itemN * sizeof(BusData));
    island->busN = itemN;
    for(int i = 0; i < itemN; i++) {
        fgets(strLine, 150, fp);
#ifdef IEEE_FORMAT_DEBUG
   		printf("%s\n", strLine);
#endif
        parse_bus_data(island->buses + i, strLine);
    }

    //寻找Branch数据卡开始
    n = strlen(BRANCH_CARD_BEGIN);
    do{
        fgets(strLine, 150, fp);
    } while(strncmp(strLine, BRANCH_CARD_BEGIN, n) != 0);

    itemN = getItemN(strLine);
    //找到了
    island->branches = malloc(itemN * sizeof(BranchData));
    island->branchN = itemN;
    for(int i = 0; i < itemN; i++) {
        fgets(strLine, 150, fp);
#ifdef IEEE_FORMAT_DEBUG
        printf("%s\n", strLine);
#endif
        parse_branch_data(island->branches + i, strLine);
    }
    //todo：interChange, tieLine, loassZone data are not parsed!
    fclose(fp);
}

void write_island_file(IEEEDataIsland * island, char * filePath) {
    FILE * fp;
    if((fp = fopen(filePath, "wt")) == NULL) {
       printf("Cannot open file %s!\n", filePath);
       return 0;
    }
	char buffer[128];
	write_title_data(&island->title, buffer);
	fprintf(fp, "%s\n", buffer);

	fprintf(fp, "BUS DATA FOLLOWS                    %d ITEMS\n", island->busN);
	for(int i = 0; i < island->busN; i++) {
		write_bus_data(island->buses + i, buffer);
		fprintf(fp, "%s\n", buffer);
	}
	fprintf(fp, "%s\n", BUS_CARD_END);


	fprintf(fp, "BRANCH DATA FOLLOWS                    %d ITEMS\n", island->branchN);
	for(int i = 0; i < island->branchN; i++) {
		write_branch_data(island->branches + i, buffer);
        fprintf(fp, "%s\n", buffer);
	}
	fprintf(fp, "%s\n", BRANCH_CARD_END);

	fprintf(fp, "LOSS ZONES FOLLOWS   %d ITEMS", island->lossZoneN);
	for(int i = 0; i < island->lossZoneN; i++) {
		write_loss_zone_data(island->lossZones + i, buffer);
        fprintf(fp, "%s\n", buffer);
	}
	fprintf(fp, "%s\n", LOSS_ZONE_CARD_END);

	fprintf(fp, "INTERCHANGE DATA FOLLOWS      %d ITEMS\n", island->interchangeN);
	for(int i = 0; i < island->interchangeN; i++) {
		write_interchange_data(island->interChanges + i, buffer);
		fprintf(fp, "%s\n", buffer);
	}
	fprintf(fp, "%s\n", INTERCHANGE_CARD_END);

	fprintf(fp, "TIE LINES FOLLOWS             %d ITEMS\n", island->tieLineN);
	for(int i = 0; i < island->tieLineN; i++) {
		write_interchange_data(island->tieLines + i, buffer);
        fprintf(fp, "%s\n", buffer);
	}
	fprintf(fp, "%s\n", TIE_LINE_CARD_END);

	fprintf(fp, "END OF DATA\n");
	fclose(fp);
}

void free_island(IEEEDataIsland * island) {
    if(island->buses != NULL)
        free(island->buses);
    if(island->branches != NULL)
        free(island->branches);
    if(island->lossZones != NULL)
        free(island->lossZones);
    if(island->tieLines != NULL)
        free(island->tieLines);
    if(island->interChanges != NULL)
        free(island->interChanges);
}