<template>
    <div class="metric-selector">
        <label>指标选择:</label>
        <select id="metric-select" v-model="modelValue" @change="$emit('update:modelValue', modelValue)">
            <option value="task_clock">CPU时间(task_clock)</option>
            <option value="ts">持续时间(ts)</option>
            <option value="alloc">内存分配(alloc)</option>
            <option value="dealloc">内存释放(dealloc)</option>
        </select>
    </div>
</template>

<script setup>
import { ref } from 'vue'

const props = defineProps({
    modelValue: {
        type: String,
        default: 'task_clock'
    }
})

const emit = defineEmits(['update:modelValue'])

const modelValue = ref(props.modelValue)
</script>

<style scoped>
.metric-selector {
    margin-bottom: 20px;
}

.metric-selector select {
    padding: 8px;
    border-radius: 4px;
    border: 1px solid #ddd;
}
</style>
