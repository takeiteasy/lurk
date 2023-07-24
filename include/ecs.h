//
//  ecs.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef ecs_h
#define ecs_h
#include <stdio.h>
#include <assert.h>
#include "bool.h"
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "maths.h"
#include "sokol_time.h"

// Taken from: https://gist.github.com/61131/7a22ac46062ee292c2c8bd6d883d28de
#define N_ARGS(...) _NARG_(__VA_ARGS__, _RSEQ())
#define _NARG_(...) _SEQ(__VA_ARGS__)
#define _SEQ(_1, _2, _3, _4, _5, _6, _7, _8, _9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,_64,_65,_66,_67,_68,_69,_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_90,_91,_92,_93,_94,_95,_96,_97,_98,_99,_100,_101,_102,_103,_104,_105,_106,_107,_108,_109,_110,_111,_112,_113,_114,_115,_116,_117,_118,_119,_120,_121,_122,_123,_124,_125,_126,_127,N,...) N
#define _RSEQ() 127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

typedef union {
    struct {
        uint32_t id;
        uint16_t version;
        uint8_t unused;
        uint8_t flag;
    } parts;
    uint64_t id;
} Entity;

#define ENTITY_ID(E) \
    ((E).parts.id)
#define ENTITY_VERSION(E) \
    ((E).parts.version)
#define ENTITY_IS_NIL(E) \
    ((E).parts.id == EcsNil)
#define ENTITY_CMP(A, B) \
    ((A).id == (B).id)

#define EcsNil 0xFFFFFFFFull
#define EcsNilEntity (Entity){.id = EcsNil}

#define ECS_COMPONENT(T) EcsNewComponent(sizeof(T))
#define ECS_TAG(WORLD) EcsNewComponent(0)
#define ECS_QUERY(CB, UD, ...) EcsQuery(CB, UD, (Entity[]){__VA_ARGS__}, sizeof((Entity[]){__VA_ARGS__}) / sizeof(Entity));
#define ECS_FIELD(QUERY, T, IDX) (T*)EcsQueryField(QUERY, IDX)
#define ECS_SYSTEM(CB, ...) EcsNewSystem(CB, N_ARGS(__VA_ARGS__), __VA_ARGS__)
#define ECS_PREFAB(...) EcsNewPrefab(N_ARGS(__VA_ARGS__), __VA_ARGS__)
#define ECS_CHILDREN(PARENT, CB) (EcsRelations((PARENT), EcsChildof, (CB)))
#define ENTITY_ISA(E, TYPE) ((E).parts.flag == Ecs##TYPE##Type)

typedef struct {
    Entity *sparse;
    Entity *dense;
    size_t sizeOfSparse;
    size_t sizeOfDense;
} EcsSparse;

typedef struct {
    Entity componentId;
    void *data;
    size_t sizeOfData;
    size_t sizeOfComponent;
    EcsSparse *sparse;
} EcsStorage;

typedef struct {
    EcsStorage **storages;
    size_t sizeOfStorages;
    Entity *entities;
    size_t sizeOfEntities;
    uint32_t *recyclable;
    size_t sizeOfRecyclable;
    uint32_t nextAvailableId;
} EcsWorld;

extern EcsWorld world;

typedef struct {
    void **componentData;
    Entity *componentIndex;
    size_t sizeOfComponentData;
    Entity entity;
    void *userdata;
} Query;

typedef void(*SystemCb)(Query *query);

typedef struct {
    Entity *components;
    size_t sizeOfComponents;
} Prefab;

typedef struct {
    SystemCb callback;
    Entity *components;
    size_t sizeOfComponents;
    bool enabled;
} System;

typedef struct {
    Entity object,
           relation;
} Relation;

typedef void(*TimerCb)(void*);

typedef struct {
    uint64_t start;
    int interval;
    TimerCb cb;
    void *userdata;
    bool enabled;
} Timer;

typedef enum {
    EcsEntityType    = 0,
    EcsComponentType = (1 << 0),
    EcsSystemType    = (1 << 1),
    EcsPrefabType    = (1 << 2),
    EcsRelationType  = (1 << 3),
    EcsTimerType     = (1 << 4)
} EntityFlag;

// MARK: ECS Functions

Entity EcsNewEntity(void);
Entity EcsNewComponent(size_t sizeOfComponent);
Entity EcsNewSystem(SystemCb fn, size_t sizeOfComponents, ...);
Entity EcsNewPrefab(size_t sizeOfComponents, ...);
Entity EcsNewTimer(int interval, bool enable, TimerCb cb, void *userdata);
void DestroyEntity(Entity entity);

bool EcsIsValid(Entity entity);
bool EcsHas(Entity entity, Entity component);
void EcsAttach(Entity entity, Entity component);
void EcsAssociate(Entity entity, Entity object, Entity relation);
void EcsDetach(Entity entity, Entity component);
void EcsDisassociate(Entity entity);
bool EcsHasRelation(Entity entity, Entity object);
bool EcsRelated(Entity entity, Entity relation);
void* EcsGet(Entity entity, Entity component);
void EcsSet(Entity entity, Entity component, const void *data);
void EcsRelations(Entity parent, Entity relation, void *userdata, SystemCb cb);

void EcsEnableSystem(Entity system);
void EcsDisableSystem(Entity system);
void EcsEnableTimer(Entity timer);
void EcsDisableTimer(Entity timer);

void EcsRunSystem(Entity system);
void EcsStep(void);
void EcsQuery(SystemCb cb, void *userdata, Entity *components, size_t sizeOfComponents);
void* EcsQueryField(Query *query, size_t index);

extern Entity EcsSystem;
extern Entity EcsPrefab;
extern Entity EcsRelation;
extern Entity EcsChildOf;
extern Entity EcsTimer;

#endif /* ecs_h */
