use serde_json::Value;
use serde::{Serialize, Deserialize};
use std::{collections::HashMap, fs::File, io::{BufReader, Read}};

#[derive(Debug, Serialize, Deserialize)]
pub struct Thread {
    pub id: u64,
    pub name: String,
    pub nodes: Vec<Node>,
    pub edges: Vec<Edge>,
    pub metrics: ThreadMetrics,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct ThreadMetrics {
    pub cpu_usage: CpuUsage,
    pub memory_usage: u64,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Metadata {
    pub schema_version: String,
    pub timestamp: u64,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct CpuUsage {
    pub system: u64,
    pub user: u64,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Process {
    pub cpu_usage: CpuUsage,
    pub memory_usage: u64,
    pub pid: u32,
    pub name: String,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Profile {
    pub metadata: Metadata,
    pub process: Process,
    pub threads: Vec<Thread>,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Node {
    pub id: u64,
    pub location: FileLocation,
    pub tag: String,
    pub in_metrics: NodeMetrics,
    pub out_metrics: NodeMetrics,
    pub self_metrics: NodeMetrics,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Edge {
    pub from: u64,
    pub to: u64,
    pub metrics: EdgeMetrics,
}

#[derive(Debug, Default, Serialize, Deserialize)]
pub struct NodeMetrics {
    pub cpu_user: u64,
    pub cpu_sys: u64,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct EdgeMetrics {
    pub cpu_user: u64,
    pub cpu_sys: u64,
}

#[derive(Debug, Serialize, Deserialize)]
pub struct FileLocation {
    pub file: String,
    pub line: u64,
}

impl Profile {
    pub fn parse(json_str: &str) -> Result<Self, Box<dyn std::error::Error>> {
        // let data: Value = serde_json::from_reader(reader)?;
        let data: Value = serde_json::from_str(&json_str)?;
        
        // 解析元数据
        let metadata = Metadata {
            schema_version: data["metadata"]["schema_version"]
                .as_str()
                .unwrap_or("unknown")
                .to_string(),
            timestamp: data["metadata"]["timestamp"].as_u64().unwrap_or_default(),
        };

        // 解析进程指标
        let process = Process {
            cpu_usage: CpuUsage {
                system: data["process"]["cpu_usage"]["system"].as_u64().unwrap_or_default(),
                user: data["process"]["cpu_usage"]["user"].as_u64().unwrap_or_default(),
            },
            memory_usage: data["process"]["memory_usage"].as_u64().unwrap_or(0),
            pid: data["process"]["pid"].as_u64().unwrap_or_default() as u32,
            name: data["process"]["name"].as_str().unwrap_or("").to_string(),
        };

        let mut threads = vec![];
        
        if let Some(json_threads) = data["threads"].as_array() {
            for json_thread in json_threads {
                // 解析线程级CPU指标
                let cpu_usage = CpuUsage {
                    system: json_thread["cpu_usage"]["system"].as_u64().unwrap_or_default(),
                    user: json_thread["cpu_usage"]["user"].as_u64().unwrap_or_default(),
                };

                let mut thread = Thread {
                    id: json_thread["id"].as_u64().unwrap_or_default(),
                    name: json_thread["name"].as_str().unwrap_or("").to_string(),
                    metrics: ThreadMetrics {
                        cpu_usage,
                        memory_usage: 0, // 预留字段
                    },
                    nodes: vec![],
                    edges: vec![],
                };

                // 处理节点
                if let Some(nodes) = json_thread["graph"]["nodes"].as_array() {
                    for node in nodes {
                        thread.nodes.push(Node {
                            id: node["id"].as_u64().unwrap(),
                            location: FileLocation {
                                file: node["location"]["file"].as_str().unwrap_or("").to_string(),
                                line: node["location"]["line"].as_u64().unwrap_or(0),
                            },
                            tag: node["tag"].as_str().unwrap_or("").to_string(),
                            in_metrics: NodeMetrics::default(),
                            out_metrics: NodeMetrics::default(),
                            self_metrics: NodeMetrics::default(),
                        });
                    }
                }

                // 处理边并聚合指标
                let mut to_node_metrics = HashMap::new();
                let mut from_node_metrics = HashMap::new();
                if let Some(edges) = json_thread["graph"]["edges"].as_array() {
                    for edge in edges {
                        let to = edge["to"].as_u64().unwrap();
                        let from = edge["from"].as_u64().unwrap();
                        let metrics = edge["metrics"].as_object().unwrap();
                        
                        // 累加节点指标
                        {
                            let entry = to_node_metrics.entry(to).or_insert((0, 0));
                            entry.0 += metrics["cpu_user"].as_u64().unwrap_or(0);
                            entry.1 += metrics["cpu_sys"].as_u64().unwrap_or(0);
                        }
                        {
                            let entry = from_node_metrics.entry(from).or_insert((0, 0));
                            entry.0 += metrics["cpu_user"].as_u64().unwrap_or(0);
                            entry.1 += metrics["cpu_sys"].as_u64().unwrap_or(0);
                        }

                        // 添加边
                        thread.edges.push(Edge {
                            from,
                            to,
                            metrics: EdgeMetrics {
                                cpu_user: metrics["cpu_user"].as_u64().unwrap_or(0),
                                cpu_sys: metrics["cpu_sys"].as_u64().unwrap_or(0),
                            },
                        });
                    }
                }

                // 回写节点指标
                for node in &mut thread.nodes {
                    if let Some((user, sys)) = to_node_metrics.get(&node.id) {
                        node.in_metrics.cpu_user = *user;
                        node.in_metrics.cpu_sys = *sys;
                    }
                    if let Some((user, sys)) = from_node_metrics.get(&node.id) {
                        node.out_metrics.cpu_user = *user;
                        node.out_metrics.cpu_sys = *sys;
                    }
                    if node.tag == "root" {
                        node.in_metrics.cpu_user = thread.metrics.cpu_usage.user;
                        node.in_metrics.cpu_sys = thread.metrics.cpu_usage.system;
                    }
                    if (node.in_metrics.cpu_user > node.out_metrics.cpu_user) {
                        node.self_metrics.cpu_user = node.in_metrics.cpu_user - node.out_metrics.cpu_user;
                    } else {
                        node.self_metrics.cpu_user = 0;
                    }
                    if (node.in_metrics.cpu_sys > node.out_metrics.cpu_sys) {
                        node.self_metrics.cpu_sys = node.in_metrics.cpu_sys - node.out_metrics.cpu_sys;    
                    } else {
                        node.self_metrics.cpu_sys = 0;
                    }
                }

                threads.push(thread);
            }
        }

        Ok(Profile { metadata, process, threads })
    }
}
