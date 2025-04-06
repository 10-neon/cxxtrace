use clap::{Parser, Subcommand};
use std::io::{BufReader, Read};
use std::fs::File;
use std::fs;
use std::collections::HashSet;
use scopebox::models;
use scopebox::dot_generator::Generator;
use scopebox::dot_generator::DotGenerator;

#[derive(Parser)]
#[command(name = "scopebox")]
#[command(about = "性能分析数据转换工具", version = "0.1.0")]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

// 修复Cli结构定义
#[derive(Subcommand)]
enum Commands {
    Convert {
        #[arg(short, long, default_value = "trace.json")]
        input: String,
        #[arg(short, long, default_value = "output.dot")]
        output: String,
        #[arg(short, long, default_value = "dot")]
        format: String,
        #[arg(long, default_value = "cpu_user,cpu_sys")]
        metrics: String,
    }
}



// 修复main函数签名


fn main() -> Result<(), Box<dyn std::error::Error>> {
    let cli = Cli::parse();
    
    match cli.command {
        Commands::Convert { input, output, format, metrics } => {
            let enabled_metrics: HashSet<String> = metrics.split(',').map(|s| s.to_string()).collect();
            let json_str = fs::read_to_string(&input)?;
            let profile = models::Profile::parse(&json_str)?;

            match format.as_str() {
                "dot" => {
                    // let dot = dot_generator::generate(&profile, &enabled_metrics);
                    let dot = DotGenerator::generate(&profile, &enabled_metrics);
                    std::fs::write(&output, dot)?;
                    println!("DOT graph generated and saved to {}", output);
                },
                "json" => {
                    let file = File::create(&output)?;
                    serde_json::to_writer_pretty(file, &profile)?;
                },
                "html" => {
                    println!("Unsupported format: {}", format)
                },
                _ => println!("Unsupported format: {}", format),
            }
            
            ()
        }
    }
    Ok(())
}
