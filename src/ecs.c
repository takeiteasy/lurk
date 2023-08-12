//
//  ecs.c
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#include "wee.h"

#define SAFE_FREE(X)    \
    do                  \
    {                   \
        if ((X))        \
        {               \
            free((X));  \
            (X) = NULL; \
        }               \
    } while (0)

#if defined(DEBUG)
#define ASSERT(X) \
    do {                                                                                       \
        if (!(X)) {                                                                            \
            fprintf(stderr, "ERROR! Assertion hit! %s:%s:%d\n", __FILE__, __func__, __LINE__); \
            assert(false);                                                                     \
        }                                                                                      \
    } while(0)
#define ECS_ASSERT(X, Y, V)                                                                    \
    do {                                                                                       \
        if (!(X)) {                                                                            \
            fprintf(stderr, "ERROR! Assertion hit! %s:%s:%d\n", __FILE__, __func__, __LINE__); \
            Dump##Y(V);                                                                        \
            assert(false);                                                                     \
        }                                                                                      \
    } while(0)

static void DumpEntity(Entity e) {
    printf("(%llx: %d, %d, %d)\n", e.id, e.parts.id, e.parts.version, e.parts.flag);
}

static void DumpSparse(EcsSparse *sparse) {
    printf("*** DUMP SPARSE ***\n");
    printf("sizeOfSparse: %zu, sizeOfDense: %zu\n", sparse->sizeOfSparse, sparse->sizeOfDense);
    printf("Sparse Contents:\n");
    for (int i = 0; i < sparse->sizeOfSparse; i++)
        DumpEntity(sparse->sparse[i]);
    printf("Dense Contents:\n");
    for (int i = 0; i < sparse->sizeOfDense; i++)
        DumpEntity(sparse->dense[i]);
    printf("*** END SPARSE DUMP ***\n");
}

static void DumpStorage(EcsStorage *storage) {
    printf("*** DUMP STORAGE ***\n");
    printf("componentId: %u, sizeOfData: %zu, sizeOfComponent: %zu\n",
           storage->componentId.parts.id, storage->sizeOfData, storage->sizeOfComponent);
    DumpSparse(storage->sparse);
    printf("*** END STORAGE DUMP ***\n");
}
#else
#define ECS_ASSERT(X, _, __) assert(X)
#define ASSERT(X) assert(X)
static void DumpEntity(Entity e) {}
static void DumpSparse(EcsSparse *sparse) {}
static void DumpStorage(EcsStorage *storage) {}
#endif

#define ECS_COMPOSE_ENTITY(ID, VER, TAG) \
    (Entity)                             \
    {                                    \
        .parts = {                       \
            .id = ID,                    \
            .version = VER,              \
            .flag = TAG                  \
        }                                \
    }

Entity EcsSystem   = EcsNilEntity;
Entity EcsPrefab   = EcsNilEntity;
Entity EcsRelation = EcsNilEntity;
Entity EcsChildOf  = EcsNilEntity;
Entity EcsTimer    = EcsNilEntity;

EcsWorld world;

static EcsSparse* NewSparse(void) {
    EcsSparse *result = malloc(sizeof(EcsSparse));
    *result = (EcsSparse){0};
    return result;
}

static bool SparseHas(EcsSparse *sparse, Entity e) {
    ASSERT(sparse);
    uint32_t id = ENTITY_ID(e);
    ASSERT(id != EcsNil);
    return (id < sparse->sizeOfSparse) && (ENTITY_ID(sparse->sparse[id]) != EcsNil);
}

static void SparseEmplace(EcsSparse *sparse, Entity e) {
    ASSERT(sparse);
    uint32_t id = ENTITY_ID(e);
    ASSERT(id != EcsNil);
    if (id >= sparse->sizeOfSparse) {
        const size_t newSize = id + 1;
        sparse->sparse = realloc(sparse->sparse, newSize * sizeof * sparse->sparse);
        for (size_t i = sparse->sizeOfSparse; i < newSize; i++)
            sparse->sparse[i] = EcsNilEntity;
        sparse->sizeOfSparse = newSize;
    }
    sparse->sparse[id] = (Entity) { .parts = { .id = (uint32_t)sparse->sizeOfDense } };
    sparse->dense = realloc(sparse->dense, (sparse->sizeOfDense + 1) * sizeof * sparse->dense);
    sparse->dense[sparse->sizeOfDense++] = e;
}

static size_t SparseRemove(EcsSparse *sparse, Entity e) {
    ASSERT(sparse);
    ECS_ASSERT(SparseHas(sparse, e), Sparse, sparse);
    
    const uint32_t id = ENTITY_ID(e);
    uint32_t pos = ENTITY_ID(sparse->sparse[id]);
    Entity other = sparse->dense[sparse->sizeOfDense-1];
    
    sparse->sparse[ENTITY_ID(other)] = (Entity) { .parts = { .id = pos } };
    sparse->dense[pos] = other;
    sparse->sparse[id] = EcsNilEntity;
    sparse->dense = realloc(sparse->dense, --sparse->sizeOfDense * sizeof * sparse->dense);
    
    return pos;
}

static size_t SparseAt(EcsSparse *sparse, Entity e) {
    ASSERT(sparse);
    uint32_t id = ENTITY_ID(e);
    ASSERT(id != EcsNil);
    return ENTITY_ID(sparse->sparse[id]);
}

static EcsStorage* NewStorage(Entity id, size_t sz) {
    EcsStorage *result = malloc(sizeof(EcsStorage));
    *result = (EcsStorage) {
        .componentId = id,
        .sizeOfComponent = sz,
        .sizeOfData = 0,
        .data = NULL,
        .sparse = NewSparse()
    };
    return result;
}

static bool StorageHas(EcsStorage *storage, Entity e) {
    ASSERT(storage);
    ASSERT(!ENTITY_IS_NIL(e));
    return SparseHas(storage->sparse, e);
}

static void* StorageEmplace(EcsStorage *storage, Entity e) {
    ASSERT(storage);
    storage->data = realloc(storage->data, (storage->sizeOfData + 1) * sizeof(char) * storage->sizeOfComponent);
    storage->sizeOfData++;
    void *result = &((char*)storage->data)[(storage->sizeOfData - 1) * sizeof(char) * storage->sizeOfComponent];
    SparseEmplace(storage->sparse, e);
    return result;
}

static void StorageRemove(EcsStorage *storage, Entity e) {
    ASSERT(storage);
    size_t pos = SparseRemove(storage->sparse, e);
    memmove(&((char*)storage->data)[pos * sizeof(char) * storage->sizeOfComponent],
            &((char*)storage->data)[(storage->sizeOfData - 1) * sizeof(char) * storage->sizeOfComponent],
            storage->sizeOfComponent);
    storage->data = realloc(storage->data, --storage->sizeOfData * sizeof(char) * storage->sizeOfComponent);
}

static void* StorageAt(EcsStorage *storage, size_t pos) {
    ASSERT(storage);
    ECS_ASSERT(pos < storage->sizeOfData, Storage, storage);
    return &((char*)storage->data)[pos * sizeof(char) * storage->sizeOfComponent];
}

static void* StorageGet(EcsStorage *storage, Entity e) {
    ASSERT(storage);
    ASSERT(!ENTITY_IS_NIL(e));
    return StorageAt(storage, SparseAt(storage->sparse, e));
}

bool EcsIsValid(Entity e) {
        uint32_t id = ENTITY_ID(e);
    return id < world.sizeOfEntities && ENTITY_CMP(world.entities[id], e);
}

static Entity EcsNewEntityType(uint8_t type) {
        if (world.sizeOfRecyclable) {
        uint32_t idx = world.recyclable[world.sizeOfRecyclable-1];
        Entity e = world.entities[idx];
        Entity new = ECS_COMPOSE_ENTITY(ENTITY_ID(e), ENTITY_VERSION(e), type);
        world.entities[idx] = new;
        world.recyclable = realloc(world.recyclable, --world.sizeOfRecyclable * sizeof(uint32_t));
        return new;
    } else {
        world.entities = realloc(world.entities, ++world.sizeOfEntities * sizeof(Entity));
        Entity e = ECS_COMPOSE_ENTITY((uint32_t)world.sizeOfEntities-1, 0, type);
        world.entities[world.sizeOfEntities-1] = e;
        return e;
    }
}

Entity EcsNewEntity(void) {
    return EcsNewEntityType(EcsEntityType);
}

static EcsStorage* EcsFind(Entity e) {
    for (int i = 0; i < world.sizeOfStorages; i++)
        if (ENTITY_ID(world.storages[i]->componentId) == ENTITY_ID(e))
            return world.storages[i];
    return NULL;
}

static EcsStorage* EcsAssure(Entity componentId, size_t sizeOfComponent) {
    EcsStorage *found = EcsFind(componentId);
    if (found)
        return found;
    EcsStorage *new = NewStorage(componentId, sizeOfComponent);
    world.storages = realloc(world.storages, (world.sizeOfStorages + 1) * sizeof * world.storages);
    world.storages[world.sizeOfStorages++] = new;
    return new;
}

bool EcsHas(Entity entity, Entity component) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(component), Entity, component);
    return StorageHas(EcsFind(component), entity);
}

Entity EcsNewComponent(size_t sizeOfComponent) {
    Entity e = EcsNewEntityType(EcsComponentType);
    return EcsAssure(e, sizeOfComponent) ? e : EcsNilEntity;
}

Entity EcsNewSystem(SystemCb fn, size_t sizeOfComponents, ...) {
    Entity e = EcsNewEntityType(EcsSystemType);
    EcsAttach(e, EcsSystem);
    System *c = EcsGet(e, EcsSystem);
    c->callback = fn;
    c->sizeOfComponents = sizeOfComponents;
    c->components = malloc(sizeof(Entity) * sizeOfComponents);
    c->enabled = true;
    
    va_list args;
    va_start(args, sizeOfComponents);
    for (int i = 0; i < sizeOfComponents; i++)
        c->components[i] = va_arg(args, Entity);
    va_end(args);
    return e;
}

Entity EcsNewPrefab(size_t sizeOfComponents, ...) {
    Entity e = EcsNewEntityType(EcsPrefabType);
    EcsAttach(e, EcsPrefab);
    Prefab *c = EcsGet(e, EcsPrefab);
    c->sizeOfComponents = sizeOfComponents;
    c->components = malloc(sizeof(Entity) * sizeOfComponents);
    
    va_list args;
    va_start(args, sizeOfComponents);
    for (int i = 0; i < sizeOfComponents; i++)
        c->components[i] = va_arg(args, Entity);
    va_end(args);
    return e;
}

Entity EcsNewTimer(int interval, bool enable, TimerCb cb, void *userdata) {
    Entity e = EcsNewEntityType(EcsTimerType);
    EcsAttach(e, EcsTimer);
    Timer *timer = EcsGet(e, EcsTimer);
    timer->start = stm_now();
    timer->enabled = enable;
    timer->interval = Max(interval, 1);
    timer->cb = cb;
    timer->userdata = userdata;
    return e;
}
#define DEL_TYPES \
    X(System, 0)  \
    X(Prefab, 1)

void DestroyEntity(Entity e) {
        ECS_ASSERT(EcsIsValid(e), Entity, e);
    switch (e.parts.flag) {
#define X(TYPE, _)                                 \
        case Ecs##TYPE##Type: {                    \
            TYPE *s = EcsGet(e, Ecs##TYPE); \
            if (s && s->components)                \
                free(s->components);               \
            break;                                 \
        }
        DEL_TYPES
#undef X
    }
    for (size_t i = world.sizeOfStorages; i; --i)
        if (world.storages[i - 1] && SparseHas(world.storages[i - 1]->sparse, e))
            StorageRemove(world.storages[i - 1], e);
    uint32_t id = ENTITY_ID(e);
    world.entities[id] = ECS_COMPOSE_ENTITY(id, ENTITY_VERSION(e) + 1, 0);
    world.recyclable = realloc(world.recyclable, ++world.sizeOfRecyclable * sizeof(uint32_t));
    world.recyclable[world.sizeOfRecyclable-1] = id;
}

void EcsAttach(Entity entity, Entity component) {
    switch (component.parts.flag) {
        case EcsRelationType: // Use EcsRelation()
        case EcsSystemType: // NOTE: potentially could be used for some sort of event system
        case EcsTimerType:
            ASSERT(false);
        case EcsPrefabType: {
            Prefab *c = EcsGet(component, EcsPrefab);
            for (int i = 0; i < c->sizeOfComponents; i++) {
                if (ENTITY_IS_NIL(c->components[i]))
                    break;
                EcsAttach(entity, c->components[i]);
            }
            break;
        }
        case EcsComponentType:
        default: {
            ECS_ASSERT(EcsIsValid(entity), Entity, entity);
            ECS_ASSERT(EcsIsValid(component), Entity, component);
            EcsStorage *storage = EcsFind(component);
            ASSERT(storage);
            StorageEmplace(storage, entity);
            break;
        }
    }
}

void EcsAssociate(Entity entity, Entity object, Entity relation) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(object), Entity, object);
    ECS_ASSERT(ENTITY_ISA(object, Component), Entity, object);
    ECS_ASSERT(EcsIsValid(relation), Entity, relation);
    ECS_ASSERT(ENTITY_ISA(relation, Entity), Entity, relation);
    EcsAttach(entity, EcsRelation);
    Relation *pair = EcsGet(entity, EcsRelation);
    pair->object = object;
    pair->relation = relation;
}

void EcsDetach(Entity entity, Entity component) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(component), Entity, component);
    EcsStorage *storage = EcsFind(component);
    ASSERT(storage);
    ECS_ASSERT(StorageHas(storage, entity), Storage, storage);
    StorageRemove(storage, entity);
}

void EcsDisassociate(Entity entity) {
    ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsHas(entity, EcsRelation), Entity, entity);
    EcsDetach(entity, EcsRelation);
}

bool EcsHasRelation(Entity entity, Entity object) {
    ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(object), Entity, object);
    EcsStorage *storage = EcsFind(EcsRelation);
    if (!storage)
        return false;
    Relation *relation = StorageGet(storage, entity);
    if (!relation)
        return false;
    return ENTITY_CMP(relation->object, object);
}

bool EcsRelated(Entity entity, Entity relation) {
    ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(relation), Entity, relation);
    EcsStorage *storage = EcsFind(EcsRelation);
    if (!storage)
        return false;
    Relation *_relation = StorageGet(storage, entity);
    if (!_relation)
        return false;
    return ENTITY_CMP(_relation->relation, relation);
}

void* EcsGet(Entity entity, Entity component) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(component), Entity, component);
    EcsStorage *storage = EcsFind(component);
    ASSERT(storage);
    return StorageHas(storage, entity) ? StorageGet(storage, entity) : NULL;
}

void EcsSet(Entity entity, Entity component, const void *data) {
        ECS_ASSERT(EcsIsValid(entity), Entity, entity);
    ECS_ASSERT(EcsIsValid(component), Entity, component);
    EcsStorage *storage = EcsFind(component);
    ASSERT(storage);
    
    void *componentData = StorageHas(storage, entity) ?
                                    StorageGet(storage, entity) :
                                    StorageEmplace(storage, entity);
    ASSERT(componentData);
    memcpy(componentData, data, storage->sizeOfComponent);
}

static void DestroyQuery(Query *query) {
    SAFE_FREE(query->componentIndex);
    SAFE_FREE(query->componentData);
}

void EcsRelations(Entity parent, Entity relation, void *userdata, SystemCb cb) {
    EcsStorage *pairs = EcsFind(EcsRelation);
    for (size_t i = 0; i < world.sizeOfEntities; i++) {
        Entity e = world.entities[i];
        if (!StorageHas(pairs, e))
            continue;
        Relation *pair = StorageGet(pairs, e);
        if (!ENTITY_CMP(pair->object, relation) || !ENTITY_CMP(pair->relation, parent))
            continue;
        Query query = {
            .entity = e,
            .componentData = malloc(sizeof(void*)),
            .componentIndex = malloc(sizeof(Entity)),
            .sizeOfComponentData = 1,
            .userdata = userdata
        };
        query.componentIndex[0] = relation;
        query.componentData[0] = (void*)pair;
        cb(&query);
        DestroyQuery(&query);
    }
}

void EcsEnableSystem(Entity system) {
    ECS_ASSERT(EcsIsValid(system), Entity, system);
    ECS_ASSERT(ENTITY_ISA(system, System), Entity, system);
    System *s = EcsGet(system, EcsSystem);
    s->enabled = true;
}

void EcsDisableSystem(Entity system) {
    ECS_ASSERT(EcsIsValid(system), Entity, system);
    ECS_ASSERT(ENTITY_ISA(system, System), Entity, system);
    System *s = EcsGet(system, EcsSystem);
    s->enabled = false;
}

void EcsEnableTimer(Entity timer) {
    ECS_ASSERT(EcsIsValid(timer), Entity, timer);
    ECS_ASSERT(ENTITY_ISA(timer, Timer), Entity, timer);
    Timer *t = EcsGet(timer, EcsTimer);
    t->enabled = true;
    t->start = stm_now();
}

void EcsDisableTimer(Entity timer) {
    ECS_ASSERT(EcsIsValid(timer), Entity, timer);
    ECS_ASSERT(ENTITY_ISA(timer, Timer), Entity, timer);
    Timer *t = EcsGet(timer, EcsTimer);
    t->enabled = false;
}

void EcsRunSystem(Entity e) {
    ECS_ASSERT(EcsIsValid(e), Entity, e);
    ECS_ASSERT(ENTITY_ISA(e, System), Entity, e);
    System *system = EcsGet(e, EcsSystem);
    EcsQuery(system->callback, NULL, system->components, system->sizeOfComponents);
}

void EcsStep(void) {
    EcsStorage *storage = world.storages[ENTITY_ID(EcsSystem)];
    for (int i = 0; i < storage->sparse->sizeOfDense; i++) {
        System *system = StorageGet(storage, storage->sparse->dense[i]);
        if (system->enabled)
            EcsQuery(system->callback, NULL, system->components, system->sizeOfComponents);
    }
}

void EcsQuery(SystemCb cb, void *userdata, Entity *components, size_t sizeOfComponents) {
    for (size_t e = 0; e < world.sizeOfEntities; e++) {
        bool hasComponents = true;
        Query query = {
            .componentData = NULL,
            .componentIndex = NULL,
            .sizeOfComponentData = 0,
            .entity = world.entities[e],
            .userdata = userdata
        };
        
        for (size_t i = 0; i < sizeOfComponents; i++) {
            EcsStorage *storage = EcsFind(components[i]);
            if (!StorageHas(storage, world.entities[e])) {
                hasComponents = false;
                break;
            }
            
            query.sizeOfComponentData++;
            query.componentData = realloc(query.componentData, query.sizeOfComponentData * sizeof(void*));
            query.componentIndex = realloc(query.componentIndex, query.sizeOfComponentData * sizeof(Entity));
            query.componentIndex[query.sizeOfComponentData-1] = components[i];
            query.componentData[query.sizeOfComponentData-1] = StorageGet(storage, world.entities[e]);
        }
        
        if (hasComponents)
            cb(&query);
        DestroyQuery(&query);
    }
}

void* EcsQueryField(Query *query, size_t index) {
    return index >= query->sizeOfComponentData || ENTITY_IS_NIL(query->componentIndex[index]) ? NULL : query->componentData[index];
}
