import { defineStore } from 'pinia'
import { buildAllThreadFlamegraph, buildTagsCost } from '../utils/traceProcessor'

export const useTraceStore = defineStore('trace', {
  state: () => ({
    traceData: null,
    flamegraphs: null
  }),
  actions: {
    setTraceData(data) {
        this.traceData = data
        this.flamegraphs ={
          'ts': buildAllThreadFlamegraph(data, 'ts'),
          'task_clock': buildAllThreadFlamegraph(data, 'task_clock'),
          'alloc': buildAllThreadFlamegraph(data, 'alloc'),
          'dealloc': buildAllThreadFlamegraph(data, 'dealloc')
        },
        this.tags_self_cost = {
            'ts': buildTagsCost(this.flamegraphs['ts']),
            'task_clock': buildTagsCost(this.flamegraphs['task_clock']),
            'alloc': buildTagsCost(this.flamegraphs['alloc']),
            'dealloc': buildTagsCost(this.flamegraphs['dealloc'])
        }
    }
  },
  getters: {
    getFlamegraphs: (state) => state.flamegraphs
  }
})
