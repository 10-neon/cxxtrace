<script setup>
import FileSelector from '@/components/FileSelector.vue'
import { useTraceStore } from '@/stores/trace'

const traceStore = useTraceStore()

const handleFileChange = (trace_events) => {
    console.log('Trace events loaded:', trace_events)
    traceStore.setTraceData(trace_events)
}
</script>

<template>
  <div>
    <FileSelector @file-selected="handleFileChange" />
  </div>
</template>

<style scoped>
</style>
