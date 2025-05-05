<template>
    <div>
        <MetricSelector v-model="metric" @update:modelValue="updatePie" />
        <ThreadSelector v-model="selectedThreads" @update:modelValue="updatePie" />
        <div ref="pieChart" style="width: 600px; height: 400px;"></div>
    </div>
</template>

<script setup>
import { ref, onMounted, watch, nextTick } from 'vue'
import { useTraceStore } from '../stores/trace'
import * as echarts from 'echarts'
import MetricSelector from '../components/MetricSelector.vue'
import ThreadSelector from '../components/ThreadSelector.vue'

const traceStore = useTraceStore()
const metric = ref('task_clock')
const selectedThreads = ref([])
const pieChart = ref(null)
let chartInstance = null

const updatePie = () => {
    if (!traceStore.tags_self_cost || !pieChart.value) return

    // 过滤数据
    const metricData = traceStore.tags_self_cost[metric.value] || []
    const filteredData = selectedThreads.value.length > 0
        ? metricData.filter(item => selectedThreads.value.includes(item.tid))
        : metricData

    // 聚合tag成本
    const tagCostMap = {}
    filteredData.forEach(item => {
        if (!tagCostMap[item.tag]) {
            tagCostMap[item.tag] = 0
        }
        tagCostMap[item.tag] += item.cost
    })

    // 转换为饼图数据格式
    const pieData = Object.entries(tagCostMap).map(([name, value]) => ({
        name,
        value
    }))

    // 渲染图表
    if (!chartInstance) {
        chartInstance = echarts.init(pieChart.value)
    }

    chartInstance.setOption({
        title: {
            text: 'Tag Cost Distribution',
            left: 'center'
        },
        tooltip: {
            trigger: 'item',
            formatter: '{a} <br/>{b}: {c} ({d}%)'
        },
        legend: {
            orient: 'vertical',
            left: 'left',
            data: pieData.map(item => item.name)
        },
        series: [{
            name: 'Tag Cost',
            type: 'pie',
            radius: '50%',
            data: pieData,
            emphasis: {
                itemStyle: {
                    shadowBlur: 10,
                    shadowOffsetX: 0,
                    shadowColor: 'rgba(0, 0, 0, 0.5)'
                }
            }
        }]
    })
}

onMounted(() => {
    nextTick(() => {
        updatePie()
    })
})

watch([metric, selectedThreads], () => {
    updatePie()
})
</script>

<style scoped></style>
