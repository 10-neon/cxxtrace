/**
 * 处理trace_event数据，按线程聚类并生成火焰图和调用图所需数据结构
 * @param {Array} traceEvents - 原始trace事件数组
 * @returns {Object} 包含线程数据和转换后结构的对象
 */
export function buildAllThreadFlamegraph(traceEvents, metric) {
  // 1. 按线程ID聚类事件
  const threads = {};
  traceEvents.forEach(event => {
    if (!threads[event.tid]) {
      threads[event.tid] = [];
    }
    threads[event.tid].push(event);
  });

  Object.keys(threads).forEach(tid => {
    threads[tid].sort((a, b) => {
        if (a.ts === b.ts) {
          return a.event === 'B' ? -1 : 1;
        }
        return a.ts - b.ts;
    });
  });

  // 2. 为每个线程构建火焰图数据结构
  const flameGraphData = {};
  Object.keys(threads).forEach(tid => {
    flameGraphData[tid] = buildFlameGraph(threads[tid], metric);
  });

  return flameGraphData
}

/**
 * 构建火焰图数据结构
 * @param {Array} events - 单个线程的事件数组
 * @returns {Object} 火焰图数据结构
 */
function buildFlameGraph(events, metric) {
  const stack = [];
  const root = {name: 'root', value: 0, children: []};

  events.forEach(event => {
    if (event.event === 'B') {
      const node = {
        name: event.tag,
        value: 0,
        children: [],
        startValue: event[metric]
      };

      if (stack.length > 0) {
        stack[stack.length - 1].children.push(node);
      } else {
        root.children.push(node);
      }

      stack.push(node);
    } else if (event.event === 'E' && stack.length > 0) {
      const node = stack.pop();
      node.value = event[metric] - node.startValue;
    }
  });

  let merged_root = mergeFlameGraphNode(root);
  const sum = merged_root.children.reduce((acc, cur) => {
    acc.value += cur.value;
    return acc;
  }, {value: 0})
  merged_root.value = sum.value;
  console.log(metric)
  console.log(merged_root.value)
  return merged_root;
}

function mergeFlameGraphNode(node) {
  if (node.children.length === 0) {
    return node;
  }

  // 合并当前节点的同名子节点
  const mergedChildren = {};
  node.children.forEach(child => {
    if (!mergedChildren[child.name]) {
      mergedChildren[child.name] = {
        ...child,
      };
    } else {
      mergedChildren[child.name].value += child.value;
      mergedChildren[child.name].children.concat(child.children);
    }
  });
  node.children = Object.values(mergedChildren);
  node.children.forEach(child => {
    mergeFlameGraphNode(child);
  });
  return node;
}

function getTagsCost(node, tags_cost) {
    let self_cost = node.value;
    if (node.children.length != 0) {
        const child_sum = node.children.reduce((acc, cur) => {
            acc.value += cur.value;
            return acc;
        }, {value: 0});
        self_cost = node.value - child_sum.value;
    }
    if (!tags_cost[node.name]) {
        tags_cost[node.name] = self_cost;
    } else {
        tags_cost[node.name] += self_cost;
    }
    node.children.forEach(child => {
        getTagsCost(child, tags_cost);
    })

}
export function buildTagsCost(flamegraphs) {
    let tags_cost = []
    Object.keys(flamegraphs).forEach(tid => {
        const flamegraph = flamegraphs[tid];
        let tags_cost_map = {};
        getTagsCost(flamegraph, tags_cost_map);
        let tags_cost_list = Object.entries(tags_cost_map).map(([key, value]) => ({
            tid: tid,
            tag: key,
            cost: value
        }));
        for (let tag_cost_record of tags_cost_list) {
            tags_cost.push(tag_cost_record);
        }

    })
    return tags_cost;
}
