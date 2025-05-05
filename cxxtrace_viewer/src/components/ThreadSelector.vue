<template>
    <div class="thread-selector">
        <label>线程选择:</label>
        <select id="thread-select" v-model="selectedThreads" multiple
            @change="$emit('update:modelValue', selectedThreads)">
            <option v-for="thread in availableThreads" :key="thread" :value="thread">
                线程 {{ thread }}
            </option>
        </select>
    </div>
</template>

<script setup>
import { ref, watch } from 'vue'
import { useTraceStore } from '../stores/trace'

const props = defineProps({
    modelValue: {
        type: Array,
        default: () => []
    }
})

const emit = defineEmits(['update:modelValue'])
const traceStore = useTraceStore()
const selectedThreads = ref(props.modelValue)
const availableThreads = ref([])

watch(() => traceStore.traceData, (newVal) => {
    if (newVal) {
        let tids = {}
        newVal.forEach(item => {
            if (item.tid) {
                tids[item.tid] = true
            }
        })
        availableThreads.value = Object.keys(tids || {})
        console.log('availableThreads is', availableThreads.value)
    }
}, { immediate: true })
</script>

<style scoped>
.thread-selector {
    margin-bottom: 20px;
}

.thread-selector select {
    padding: 8px;
    border-radius: 4px;
    border: 1px solid #ddd;
    min-width: 200px;
    height: 100px;
}

.thread-selector select option:checked {
    background-color: #409eff;
    color: white;
}
</style>
