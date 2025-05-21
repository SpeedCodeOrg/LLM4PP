from omegaconf import DictConfig, OmegaConf
from hydra.utils import get_method 
import hydra

@hydra.main(version_base=None, config_path="config", config_name="main")
def main(cfg: DictConfig) -> None:
    """
    cfg.methodology now *is* the DictConfig that lives in
    configs/methodology/<chosen>.yaml, including its _target_.
    """
    # print("Selected strategy:", cfg.strategy._target_)

    print("Strategy selected:", cfg._target_.split('.')[1])
    print("Target function :", cfg._target_)

    print("Current Configuration: ")
    print(OmegaConf.to_yaml(cfg, resolve=True))   # nice for debugging

    run_strategy_fn = get_method(cfg._target_)

    run_strategy_fn(cfg)

    # Hydra 1.2+ : call() will import the target and execute it,
    # passing the *methodology* subtree (not the whole cfg) as argument.
    # call(cfg.strategy)

if __name__ == "__main__":
    main()