<template>
    <div>
        <MetricSelector v-model="metric" @update:modelValue="renderFlameGraphs(traceStore.flamegraphs)" />
        <div class="flame-graph-container" ref="flameGraphContainer">
            <div v-for="thread in threads" :key="thread.id">
                <h3 class="flame-graph-title">线程 {{ thread.id }} - {{ metric }} - 总开销 {{ thread.total_cost }}({{
                    thread.percent.toFixed(2) + '%' }})</h3>
                <div :id=get_flamegraph_id(thread.id) class="flame-graph-thread"></div>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue'
import { useTraceStore } from '../stores/trace'
import * as d3 from 'd3'
import * as d3FlameGraph from 'd3-flame-graph'
import MetricSelector from '../components/MetricSelector.vue'

const traceStore = useTraceStore()
const flameGraphContainer = ref(null)
const threads = ref([])
const get_flamegraph_id = (threadId) => {
    return `flame-graph-${threadId}`
}
const metric = ref('task_clock')

const renderFlameGraphs = (flamegraphs) => {
    if (!flamegraphs || Object.keys(flamegraphs).length === 0) return

    threads.value = []
    let all_thread_total_cost = 0
    Object.keys(flamegraphs[metric.value]).forEach((threadId) => {
        console.log('threadId', threadId)
        const flamegraph = flamegraphs[metric.value][threadId]
        all_thread_total_cost += flamegraph.value
    })
    Object.keys(flamegraphs[metric.value]).forEach((threadId) => {
        console.log('threadId', threadId)
        const flamegraph = flamegraphs[metric.value][threadId]
        console.log('flamegraph', flamegraph)
        if (!flamegraph) return
        threads.value.push({ id: threadId, flamegraph: flamegraph, total_cost: flamegraph.value, percent: 100 * flamegraph.value / all_thread_total_cost })
        setTimeout(() => {
            const graph = d3FlameGraph.flamegraph()
                .width(800)
                .height(100)
                .tooltip(true)

            console.log('graph', graph)
            d3.select(`#flame-graph-${threadId}`)
                .datum(flamegraph)
                .call(graph)
        }, 0);

    })
}



onMounted(() => {
    watch(() => traceStore.flamegraphs, (flamegraphs) => {
        if (flamegraphs) {
            renderFlameGraphs(flamegraphs)
        }
    }, { immediate: true })
})
</script>

<style scoped>
.flame-graph-container {
    padding: 20px;
}

.thread-selector {
    margin-bottom: 20px;
}

.thread-selector select {
    padding: 8px;
    border-radius: 4px;
    border: 1px solid #ddd;
}

.flame-graph-container {
    display: flex;
    flex-direction: column;
    gap: 20px;
    padding: 20px;
    height: 100vh;
}

.flame-graph-thread {
    width: 100%;
    height: auto;
    min-height: 200px;
    border: 1px solid #eee;
    border-radius: 4px;
}

.flame-graph-title {
    margin-bottom: 10px;
    font-size: 16px;
    font-weight: bold;
}
</style>
