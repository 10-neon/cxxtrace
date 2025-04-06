use super::models::{Profile, Thread, Node, Edge};
use std::collections::HashSet;
use std::path::Path;

pub trait Generator {
    fn generate(profile: &Profile, enabled: &HashSet<String>) -> String;
}

pub struct DotGenerator;

impl Generator for DotGenerator {
    fn generate(profile: &Profile, _enabled: &HashSet<String>) -> String {
        let mut dot = vec![
            "digraph G {".to_string(),
            "  compound=true;".to_string(),  // 允许连接子图
            "  node [shape=box, style=rounded, fontname=\"Arial\"];".to_string(),
            "  edge [fontname=\"Arial\", fontsize=10];".to_string(),
            
            // 添加进程节点
            format!("  process_node [label=\"Process {}\\nCPU: user={}ms sys={}ms\\nMemory: {}MB\", shape=component, color=\"#28a745\", penwidth=2, style=filled, fillcolor=white];", 
                profile.process.pid,
                profile.process.cpu_usage.user,
                profile.process.cpu_usage.system,
                profile.process.memory_usage / 1_000_000
            ),
        ];

        // 生成线程子图并创建连接
        let mut thread_clusters = vec![];
        for thread in &profile.threads {
            let cluster_name = format!("cluster_thread_{}", thread.id);
            thread_clusters.push(cluster_name.clone());
            
            // 在子图中添加入口节点
            dot.push(format!(
                "  subgraph {} {{\n    \
                    label=\"{} {} (CPU user={}ms sys={}ms)\";\n    \
                    style=filled;\n    fillcolor=white;\n    color=black;\n    \
                    penwidth=2;\n    \
                    {}_entry [shape=point, style=invis];",  // 隐藏入口节点
                cluster_name,
                thread.name,
                thread.id,
                thread.metrics.cpu_usage.user,
                thread.metrics.cpu_usage.system,
                cluster_name
            ));

            // Generate nodes
            for node in &thread.nodes {
                dot.push(format!(
                    "    node_{}_{} [label=\"{}\\n{}:{} | cpu_user: {}/{} cpu_sys: {}/{}\"];",
                    node.id, thread.id,
                    node.tag,
                    Path::new(node.location.file.as_str()).file_name().and_then(|n| n.to_str()).unwrap_or("").to_string() , node.location.line,
                    node.self_metrics.cpu_user, node.in_metrics.cpu_user,
                    node.self_metrics.cpu_sys, node.in_metrics.cpu_sys,
                ));
            }

            // Generate edges
            for edge in &thread.edges {
                dot.push(format!(
                    "    node_{}_{} -> node_{}_{} [label=\"cpu_user={}\\ncpu_sys={}\"];",
                    edge.from, thread.id,
                    edge.to, thread.id,
                    edge.metrics.cpu_user,
                    edge.metrics.cpu_sys
                ));
            }

            // 创建进程到线程的连线
            dot.push("    }".to_string());
            // 创建进程到线程入口的连线 (虚线，灰色)
            dot.push(format!(
                "  process_node -> {}_entry [lhead={}, color=\"#6c757d\", style=dashed];",
                cluster_name, cluster_name
            ));
        }

        dot.push("}".to_string());
        dot.join("\n")
    }
}